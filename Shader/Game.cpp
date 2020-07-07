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

	camera_->setPosition(Vector3(0.0F, 0.0F, 0.0F));
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

	result = terrain_->init(L"Resource/setup.txt", L"Resource/asphalt.dds");
	if (!result)
	{
		return false;
	}

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	position_.setPosition(Vector3(123.0F, 10.0F, -10.0F));
	position_.setRotation(Vector3::Zero);

	cloud_ = new Skyplane;
	if (!cloud_)
	{
		return false;
	}

	result = cloud_->init(L"Resource/cloud001.dds", L"Resource/perturb001.dds");
	if (!result)
	{
		Error::showDialog("skyplaneの初期化に失敗");
		return false;
	}
	//***************************************************
	//					モデル生成
	//***************************************************

	bill_ = new Model;
	result = bill_->init(L"Resource/bill.txt", L"Resource/bill.dds");
	if (!result)
	{
		return false;
	}

	bill_->setPosition(11.0F,0.0F, 7.5F);
	bill_->setRotation(0.0F, 90.0F, 0.0F);

	house_ = new Model;
	result = house_->init(L"Resource/house.txt", L"Resource/default.dds");
	if (!result)
	{
		return false;
	}
	house_->setPosition(46.0F, 1.5F, 50.0F);

	maskmodel_ = new Model;
	result = maskmodel_->init(L"Resource/cube.txt", L"Resource/dirt.dds", L"Resource/stone.dds", L"Resource/alpha.dds");
	if (!result)
	{
		return false;
	}

	maskmodel_->setPosition(42.0F, 1.5F, 50.0F);

	multimodel_ = new Model;
	result = multimodel_->init(L"Resource/cube.txt", L"Resource/stone.dds", L"Resource/dirt.dds");
	if (!result)
	{
		return false;
	}

	multimodel_->setPosition(54.0F, 1.5F, 50.0F);

	particle_ = new ParticleSystem;
	particle_->setEmitMax(1000);
	particle_->setEmitPosition(50.0F, 1.5F, 50.0F);
	particle_->setVelcity(0.01F);
	particle_->setEmitInterval(50.0F);
	result = particle_->init(L"Resource/star.dds");
	if (!result)
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
	switchWireFrame();
	particle_->update();

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
	//破棄
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE_DESTROY(cloud_);
	SAFE_DELETE_DESTROY(bill_);
	SAFE_DELETE_DESTROY(house_);
	SAFE_DELETE_DESTROY(rendertexture_);
	SAFE_DELETE_DESTROY(maskmodel_);
	SAFE_DELETE_DESTROY(multimodel_);
	SAFE_DELETE_DESTROY(particle_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
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
	Matrix view;

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();

	//ワールド上のモデル座標を計算
	world = bill_->getWorld();
	if (!(ShaderManager::getInstance()->textureRender(bill_, world, view, projection,bill_->getTexture())))
		return false;

	//ワールド上のモデル座標を計算
	world = house_->getWorld();
	if (!(ShaderManager::getInstance()->lightRender(house_, world, view, projection, house_->getTexture(), rendertexture_->getShaderResouceView(), light_)))
		return false;

	//ワールド上のモデル座標を計算
	world = Support::worldPosition(maskmodel_);
	ID3D11ShaderResourceView* masksrv[3];
	masksrv[0] = maskmodel_->getTexture(0);
	masksrv[1] = maskmodel_->getTexture(1);
	masksrv[2] = maskmodel_->getMapTexture();
	if (!(ShaderManager::getInstance()->maskRender(maskmodel_, world, view, projection, masksrv)))
		return false;


	//ワールド上のモデル座標を計算
	world = Support::worldPosition(multimodel_);
	ID3D11ShaderResourceView* multisrv[2];
	multisrv[0] = multimodel_->getTexture(0);
	multisrv[1] = multimodel_->getTexture(1);
	if (!(ShaderManager::getInstance()->multiTextureRender(multimodel_, world, view, projection, multisrv, 2)))
		return false;

	//アルファブレンドオン
	Direct3D::getInstance()->turnAlphaBlendEnable();
	Direct3D::getInstance()->turnZbufferDisable();

	//ワールド行列を取得
	world = Direct3D::getInstance()->getWorld();
	if (!(ShaderManager::getInstance()->particleRender(particle_, world, view, projection)))
		return false;

	//ブレンドオフ・Zバッファオン
	Direct3D::getInstance()->turnAlphablendDisable();
	Direct3D::getInstance()->turnZbufferEnable();

	return true;
}

bool Game::worldRender()
{
	Matrix world, projection;
	Matrix skyworld;
	Matrix view;

	//カメラ視点をレンダリング
	camera_->render();

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	skyworld = XMMatrixTranslation(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);


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
	if (!(ShaderManager::getInstance()->skyPlaneRender(cloud_, skyworld, view, projection)))
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
