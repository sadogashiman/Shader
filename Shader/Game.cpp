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


	terrain_->setPosition(0.0F, -50.0F, 0.0F);
	terrain_->setScale(1.0F);

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

	bill_->setPosition(35.0F,0.4F, 57.5F);
	bill_->setRotation(0.0F, 90.0F, 0.0F);
	bill_->setModelScale(3.0F);

	house_ = new Model;
	result = house_->init(L"Resource/house.txt", L"Resource/default.dds");
	if (!result)
	{
		return false;
	}
	house_->setPosition(23.0F, 0.4F, 20.5F);
	house_->setModelScale(7.0F);

	wire_ = false;

	return true;
}

State* Game::update()
{
	bool result;
	camera_->update();
	switchWireFrame();

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
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();

	//ワールド上のモデル座標を計算
	world = bill_->getWorldMatrix();
	if (!(ShaderManager::getInstance()->textureRender(bill_, world, view, projection,bill_->getTexture())))
		return false;

	Direct3D::getInstance()->turnCullingDisable();

	//ワールド上のモデル座標を計算
	world = house_->getWorldMatrix();
	if (!(ShaderManager::getInstance()->lightRender(house_, world, view, projection, house_->getTexture(), rendertexture_->getShaderResouceView(), light_)))
		return false;
	Direct3D::getInstance()->turnCullingEnable();

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

	world = terrain_->getWorldMatrix();

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
