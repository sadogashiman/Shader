#include "stdafx.h"
#include "Game.h"
#include "Direct3D.h"
#include "Input.h"
#include "ShaderManager.h"
#include "System.h"
#include "Timer.h"
#include "SkyDome.h"
#include "Support.h"

Game::Game()
{
}

Game::~Game()
{
}

bool Game::init()
{
	bool result;

	//カメラオブジェクトを生成
	camera_ = new Camera;
	if (!camera_)
	{
		return false;
	}

	result = camera_->init();
	if (!result)
	{
		Error::showDialog("カメラの初期化に失敗");
		return false;
	}

	camera_->setPosition(Vector3(0.0F, -2.0F, -10.0F));
	camera_->render();
	camera_->renderBaseViewMatrix();

	//ライトオブジェクトを生成
	light_ = new Light;
	if (!light_)
	{
		return false;
	}
	light_->setPosition(0.0F, 1.0F, 0.0F);
	light_->setAmbientColor(0.05F, 0.05F, 0.05F, 1.0F);
	light_->setDiffuseColor(1.0F, 1.0F, 1.0F, 1.0F);
	light_->setDirection(-0.5F, -1.0F, 0.0F);

	rendertexture_ = new Rendertexture;
	if (!rendertexture_)
	{
		return false;
	}

	result = rendertexture_->init(kWindow_Width, kWindow_Height);
	if (!result)
	{
		Error::showDialog("RenderTextureの初期化に失敗");
		return false;
	}

	sky_ = new SkyDome;
	if (!sky_)
	{
		return false;
	}

	result = sky_->init(L"Resource/skydome.txt");
	if (!result)
	{
		return false;
	}

	sky_->setApexColor(0.0f, 0.05f, 0.6f, 1.0f);
	sky_->setCentorColor(0.0f, 0.5f, 0.8f, 1.0f);

	terrain_ = new Terrain;
	if (!terrain_)
	{
		return false;
	}

	result = terrain_->init(L"Resource/setup.txt", L"Resource/dirt.dds");
	if (!result)
	{
		return false;
	}

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	position_.setPosition(Vector3(123.0F, 10.0F, -10.0F));
	position_.setRotation(Vector3::Zero);

	skyplane_ = new Skyplane;
	if (!skyplane_)
	{
		return false;
	}

	result = skyplane_->init(L"Resource/cloud001.dds", L"Resource/perturb001.dds");
	if (!result)
	{
		Error::showDialog("skyplaneの初期化に失敗");
		return false;
	}

	//モデル生成
	texmodel_ = new Model;
	result = texmodel_->init(L"Resource/cube.txt", L"Resource/stone.dds");
	if (!result)
	{
		return false;
	}

	texmodel_->setPosition(Vector3(55.0F, 1.5F, 20.0F));

	bumpmodel_ = new Model;
	result = bumpmodel_->init(L"Resource/cube.txt", L"Resource/stone.dds");
	if (!result)
	{
		return false;
	}

	bumpmodel_->setNormalTexture(L"Resource/stone_n.dds");
	bumpmodel_->setPosition(Vector3(50.0F, 1.5F, 20.0F));

	shadowmodel_ = new Model;
	result = shadowmodel_->init(L"Resource/cube.txt", L"Resource/stone.dds");
	if (!result)
	{
		return false;
	}
	shadowmodel_->setPosition(Vector3(45.0F, 1.5F, 20.0F));

	maskmodel_ = new Model;
	result = maskmodel_->init(L"Resource/cube.txt", L"Resource/dirt.dds", L"Resource/stone.dds",L"Resource/alpha.dds");
	if (!result)
	{
		return false;
	}

	maskmodel_->setPosition(Vector3(40.0F, 1.5F, 20.0F));

	multimodel_ = new Model;
	result = multimodel_->init(L"Resource/cube.txt", L"Resource/stone.dds", L"Resource/dirt.dds");
	if (!result)
	{
		return false;
	}

	multimodel_->setPosition(35.0F, 1.5F, 20.0F);

	particle_ = new ParticleSystem;
	result = particle_->init(L"Resource/star.dds");
	if(!result)
	{
		return false;
	}

	wire_ = false;

	return true;
}

State* Game::update()
{
	bool result;
	camera_->update();
	switchWireFrame(); particle_->update();

	result = render();
	if (!result)
	{
		return nullptr;
	}

	return this;
}

bool Game::render()
{
	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::CornflowerBlue);

	if (!renderToScene())
	{
		return false;
	}

	if (!worldRender())
	{
		return false;
	}

	if (!modelRender())
	{
		return false;
	}

	//描画終了
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE_DESTROY(skyplane_);
	SAFE_DELETE_DESTROY(texmodel_);
	SAFE_DELETE_DESTROY(bumpmodel_);
	SAFE_DELETE_DESTROY(shadowmodel_);
	SAFE_DELETE_DESTROY(rendertexture_);
	SAFE_DELETE_DESTROY(maskmodel_);
	SAFE_DELETE_DESTROY(multimodel_);
	SAFE_DELETE_DESTROY(particle_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Game::renderToScene()
{
	Matrix world, view, projection;

	//レンダーターゲットをセット
	rendertexture_->setRenderTarget();

	//レンダーターゲットクリア
	rendertexture_->clearRenderTarget();

	//行列を生成
	light_->generateProjection(kScreen_depth, kScreen_near);
	light_->generateView();

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	view = light_->getViewMatrix();
	projection = light_->getProjection();


	//深度情報を書き込み
	terrain_->render();
	if (!(ShaderManager::getInstance()->depthRender(terrain_->getIndexCount(), world, view, projection)))
		return false;

	world = Support::worldPosition(shadowmodel_);
	if (!(ShaderManager::getInstance()->depthRender(shadowmodel_->getIndexCount(), world, view, projection)))
		return false;

	//バックバッファにレンダーターゲットを移す
	Direct3D::getInstance()->setRenderTarget();

	//ビューポートをリセット
	Direct3D::getInstance()->resetViewPort();

	return true;
}

void Game::switchWireFrame()
{
	if (Input::getInstance()->isKeyPressed(DIK_F2))
	{
		if (wire_)
		{
			wire_ = false;
		}
		else
		{
			wire_ = true;
		}
	}
}


bool Game::modelRender()
{
	Matrix world, projection;
	Matrix skyworld;
	Matrix view;

	static float rotation = 0;

	if (rotation < 360.0F)
	{
		rotation += 0.6F;
	}
	rotation -= 360.0F;


	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	
	//ワールド上のモデル座標を計算
	world = Support::worldPosition(texmodel_);
	if (!(ShaderManager::getInstance()->textureRender(texmodel_, world, view, projection, texmodel_->getTexture())))
		return false;

	world = Support::worldPosition(bumpmodel_);
	ID3D11ShaderResourceView* srv[2];
	srv[0] = bumpmodel_->getTexture();
	srv[1] = bumpmodel_->getMapTexture();
	if (!(ShaderManager::getInstance()->bumpRender(bumpmodel_, world, view, projection, srv, light_)))
		return false;

	world = Support::worldPosition(shadowmodel_);
	if (!(ShaderManager::getInstance()->lightRender(shadowmodel_, world, view, projection, shadowmodel_->getTexture(), rendertexture_->getShaderResouceView(), light_)))
		return false;

	world = Support::worldPosition(maskmodel_);
	ID3D11ShaderResourceView* masksrv[3];
	masksrv[0] = maskmodel_->getTexture(0);
	masksrv[1] = maskmodel_->getTexture(1);
	masksrv[2] = maskmodel_->getMapTexture();
	if (!(ShaderManager::getInstance()->maskRender(maskmodel_, world, view, projection, masksrv)))
		return false;

	world = Support::worldPosition(multimodel_);
	ID3D11ShaderResourceView* multisrv[2];
	multisrv[0] = multimodel_->getTexture(0);
	multisrv[1] = multimodel_->getTexture(1);
	if (!(ShaderManager::getInstance()->multiTextureRender(multimodel_, world, view, projection, multisrv, 2)))
		return false;

	//アルファブレンドオン
	Direct3D::getInstance()->turnAlphaBlendEnable();

	world = Direct3D::getInstance()->getWorld();
	if (!(ShaderManager::getInstance()->particleRender(particle_, world, view, projection)))
		return false;
	Direct3D::getInstance()->turnAlphablendDisable();

	return true;
}

bool Game::worldRender()
{
	Matrix world, projection;
	Matrix skyworld;
	Matrix view;

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	skyworld = XMMatrixTranslation(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	//カメラ視点をレンダリング
	camera_->render();

	//Zバッファ・カリングをオフ
	Direct3D::getInstance()->turnCullingDisable();
	Direct3D::getInstance()->turnZbufferDisable();

	//スカイドームをレンダリング
	if (!(ShaderManager::getInstance()->skyDomeRender(sky_, skyworld, view, projection)))
	{
		return false;
	}

	//カリングをオン
	Direct3D::getInstance()->turnCullingEnable();

	//加算合成オン
	Direct3D::getInstance()->turnAddBlendEnable();

	//雲をレンダリング
	if (!(ShaderManager::getInstance()->skyPlaneRender(skyplane_, skyworld, view, projection)))
	{
		return false;
	}

	//合成オフ
	Direct3D::getInstance()->turnAlphablendDisable();

	//Zバッファオン
	Direct3D::getInstance()->turnZbufferEnable();
	//ワイヤーフレーム切り替え

	if (wire_)
		Direct3D::getInstance()->wireFrameEnable();

	//テレインをレンダリング
	if (!(ShaderManager::getInstance()->terrainRender(terrain_, world, view, projection, light_)))
	{
		return false;
	}

	if (wire_)
		Direct3D::getInstance()->wireFrameDisable();

	//2Dレンダリングが終了したのでZバッファを有効にする
	Direct3D::getInstance()->turnZbufferEnable();

	return true;
}
