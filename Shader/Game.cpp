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

	camera_->setPosition(0.0F, 0.0F, 0.0F);
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
	light_->setDiffuseColor(0.8F, 0.8F, 0.8F, 1.0F);
	light_->setDirection(-0.5F, -1.0F, 0.0F);
	light_->generateOrthoMatrix(300.0F, kScreen_depth, kScreen_near);
	light_->setLookAt(0.0F, 0.0F, 0.0F);

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

	result = sky_->init(L"Resource/Model/skydome.txt");
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

	result = terrain_->init(L"Resource/Setup/setup.txt", L"Resource/Texture/asphalt.dds");
	if (!result)
	{
		return false;
	}

	terrain_->setPosition(0.0F, 0.0F, 0.0F);
	terrain_->setScale(1.0F);

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	position_.setPosition(123.0F, 10.0F, -10.0F);
	position_.setRotation(Vector3::Zero);

	cloud_ = new Skyplane;
	if (!cloud_)
	{
		return false;
	}

	result = cloud_->init(L"Resource/Texture/cloud001.dds", L"Resource/Texture/perturb001.dds");
	if (!result)
	{
		Error::showDialog("skyplaneの初期化に失敗");
		return false;
	}

	//***************************************************
	//					モデル生成
	//***************************************************

	bill03_ = new Model;
	result = bill03_->init(L"Resource/Model/bill.txt", L"Resource/Texture/bill.dds");
	if (!result)
	{
		return false;
	}

	bill03_->setPosition(35.0F, 0.4F, 60.5F);
	bill03_->setRotation(0.0F, 90.0F, 0.0F);
	bill03_->setModelScale(3.0F);

	bill04_ = new Model;
	result = bill04_->init(L"Resource/Model/bill_04.txt", L"Resource/Texture/bill.dds");
	if (!result)
	{
		return false;
	}

	bill04_->setPosition(35.0F, 0.4F, 20.5F);
	bill04_->setRotation(0.0F, 90.0F, 0.0F);
	bill04_->setModelScale(3.0F);

	bill05_ = new Model;
	result = bill05_->init(L"Resource/Model/bill_05.txt", L"Resource/Texture/bill.dds");
	if (!result)
	{
		return false;
	}

	bill05_->setPosition(150.0F, 0.4F, 30.0F);
	bill05_->setRotation(0.0F, 270.0F, 0.0F);
	bill05_->setModelScale(3.0F);

	bill06_ = new Model;
	result = bill06_->init(L"Resource/Model/bill_06.txt", L"Resource/Texture/bill.dds");
	if (!result)
	{
		return false;
	}

	bill06_->setPosition(35.0F, 0.4F, 140.5F);
	bill06_->setRotation(0.0F, 90.0F, 0.0F);
	bill06_->setModelScale(3.0F);

	bill09_ = new Model;
	result = bill09_->init(L"Resource/Model/bill_09.txt", L"Resource/Texture/bill.dds");
	if (!result)
	{
		return false;
	}

	bill09_->setPosition(35.0F, 0.4F, 180.5F);
	bill09_->setRotation(0.0F, 90.0F, 0.0F);
	bill09_->setModelScale(3.0F);

	bill10_ = new Model;
	result = bill10_->init(L"Resource/Model/bill_08.txt", L"Resource/Texture/bill.dds");
	if (!result)
	{
		return false;
	}

	bill10_->setPosition(35.0F, 0.4F, 100.5F);
	bill10_->setRotation(0.0F, 90.0F, 0.0F);
	bill10_->setModelScale(3.0F);

#ifdef _DEBUG
	lightblock_ = new Model;
	result = lightblock_->init(L"Resource/Model/cube.txt", L"Resource/Texture/default.dds");
	if (!result)
	{
		return false;
	}
#endif // _DEBUG
	wire_ = false;

	return true;
}

State* Game::update()
{
	bool result;
	static float lightangle = 270.0F;
	static float lightposx = 90.0F;

	lightposx -= 0.003F * kFrameTime;
	lightangle -= 0.03F * kFrameTime;
	if (lightangle < 90.0F)
	{
		lightangle = 270.0F;
		lightposx = 90.0F;
	}


	//light_->setDirection(std::sin(XMConvertToRadians(lightangle)), std::cos(XMConvertToRadians(lightangle)), 0.0F);
	light_->setPosition(lightposx, 100.0F, 140.0F);
	light_->setLookAt(50.0F, 0.4F, 140.0F);

	camera_->update();
	switchWireFrame();
	light_->update();

#ifdef _DEBUG
	lightblock_->setPosition(light_->getPosition());
	lightblock_->setRotation(light_->getDirection());
#endif

	result = render();
	if (!result)
	{
		return nullptr;
	}

	return this;
}

bool Game::render()
{
	//深度マップレンダリング
	if (!renderToScene())
	{
		return false;
	}
	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::CornflowerBlue);

	//シーンにレンダリング
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
	SAFE_DELETE_DESTROY(bill03_);
	SAFE_DELETE_DESTROY(bill04_);
	SAFE_DELETE_DESTROY(bill06_);
	SAFE_DELETE_DESTROY(bill05_);
	SAFE_DELETE_DESTROY(bill09_);
	SAFE_DELETE_DESTROY(bill10_);
#ifdef _DEBUG
	SAFE_DELETE_DESTROY(lightblock_);
#endif // _DEBUG
	SAFE_DELETE(rendertexture_);
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
	world = Direct3D::getInstance()->getWorldMatrix();
	projection = Direct3D::getInstance()->getProjectionMatrix();
	view = camera_->getViewMatrix();

	Direct3D::getInstance()->turnCullingEnable();

	//ワールド上のモデル座標を計算
	world = bill03_->getWorldMatrix();
	if (!ShaderManager::getInstance()->shadowRender(bill03_, world, view, projection, bill03_->getTexture(), rendertexture_->getShaderResouceView(), light_))
		return false;

	//ワールド上のモデル座標を計算
	world = bill04_->getWorldMatrix();
	if (!ShaderManager::getInstance()->shadowRender(bill04_, world, view, projection, bill04_->getTexture(), rendertexture_->getShaderResouceView(), light_))
		return false;

	world = bill05_->getWorldMatrix();
	if (!ShaderManager::getInstance()->shadowRender(bill05_, world, view, projection, bill05_->getTexture(), rendertexture_->getShaderResouceView(), light_))
		return false;

	world = bill06_->getWorldMatrix();
	if (!ShaderManager::getInstance()->shadowRender(bill06_, world, view, projection, bill06_->getTexture(), rendertexture_->getShaderResouceView(), light_))
		return false;

	world = bill09_->getWorldMatrix();
	if (!ShaderManager::getInstance()->shadowRender(bill09_, world, view, projection,bill09_->getTexture(),rendertexture_->getShaderResouceView(),light_))
		return false;

	world = bill10_->getWorldMatrix();
	if (!ShaderManager::getInstance()->shadowRender(bill10_, world, view, projection, bill10_->getTexture(), rendertexture_->getShaderResouceView(), light_))
		return false;
#ifdef _DEBUG
	world = lightblock_->getWorldMatrix();
	if (!ShaderManager::getInstance()->textureRender(lightblock_, world, view, projection, lightblock_->getTexture()))
		return false;
#endif // _DEBUG

	Direct3D::getInstance()->turnCullingDisable();

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
	world = Direct3D::getInstance()->getWorldMatrix();
	projection = Direct3D::getInstance()->getProjectionMatrix();
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

	world = terrain_->getWorldMatrix();

	//テレインをレンダリング
	if (!(ShaderManager::getInstance()->terrainRender(terrain_, world, view, projection, light_)))
	{
		return false;
	}

	if (wire_)
		Direct3D::getInstance()->wireFrameDisable();

	return true;
}

bool Game::renderToScene()
{
	Matrix world;
	Matrix view;
	Matrix projection;

	Direct3D::getInstance()->setBackBufferRenderTarget();

	//描画先変更
	Direct3D::getInstance()->turnCullingDisable();
	rendertexture_->setRenderTarget();
	rendertexture_->clearRenderTarget(Colors::ForestGreen);

	//射影行列を生成
	light_->generateView();

	world = Direct3D::getInstance()->getWorldMatrix();

	//行列を取得
	view = light_->getViewMatrix();
	projection = rendertexture_->getOrthoMatrix();

	//深度マップにレンダリング
	world = terrain_->getWorldMatrix();
	terrain_->render();
	if (!ShaderManager::getInstance()->depthRender(terrain_->getIndexCount(), world, view, projection))
		return false;

	world = bill05_->getWorldMatrix();
	if (!ShaderManager::getInstance()->depthRender(bill05_, world, view, projection))
		return false;

	world = bill03_->getWorldMatrix();
	if (!ShaderManager::getInstance()->depthRender(bill03_, world, view, projection))
		return false;

	world = bill04_->getWorldMatrix();
	if (!ShaderManager::getInstance()->depthRender(bill04_, world, view, projection))
		return false;



	world = bill06_->getWorldMatrix();
	if (!ShaderManager::getInstance()->depthRender(bill06_, world, view, projection))
		return false;

	world = bill09_->getWorldMatrix();
	if (!ShaderManager::getInstance()->depthRender(bill09_, world, view, projection))
		return false;

	world = bill10_->getWorldMatrix();
	if (!ShaderManager::getInstance()->depthRender(bill10_, world, view, projection))
		return false;

	Direct3D::getInstance()->setBackBufferRenderTarget();

	Direct3D::getInstance()->resetViewPort();

	return true;
}
