#include "stdafx.h"
#include "Game.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"ShaderManager.h"
#include"System.h"
#include"Timer.h"
#include"SkyDome.h"


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
	camera_->setPosition(Vector3(0.0F, 0.0F, -10.0F));
	camera_->render();
	camera_->renderBaseViewMatrix();

	//ライトオブジェクトを生成
	light_ = new Light;
	if (!light_)
	{
		return false;
	}

	light_->setDiffuseColor(1.0F, 1.0F, 1.0F, 1.0F);
	light_->setDirection(0.0F, 0.0F, 1.0F);

	

	//バッファを作成
	defbuffer_ = new Deferredbuffers;
	if (!defbuffer_)
	{
		return false;
	}

	if (!defbuffer_->init(System::getWindowWidth(), System::getWindowHeight(), kScreen_depth, kScreen_near))
	{
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

	result = terrain_->init();
	if (!result)
	{
		return false;
	}

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	return true;

}

State* Game::update()
{
	bool result;
	handleMovementInput();

	result = render();
	if (!result)
	{
		return nullptr;
	}

	return this;
}

bool Game::render()
{
	bool result;
	Matrix world, view, projection,ortho;
	Matrix baseview;
	static float rotation = 0;

	camera_->render();

	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::Black);

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	baseview = camera_->getBaseViewMatrix();
	world = Direct3D::getInstance()->getWorld();

	result = ShaderManager::getInstance()->colorRender(terrain_, world, view, projection);
	if (!result)
	{
		return false;
	}
	
	//描画終了
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Game::renderSceneToTexture()
{
	Matrix world, view, projection;

	//レンダーターゲットを作成
	defbuffer_->setRenderTargets();

	//レンダーバッファをクリア
	defbuffer_->clearRenderTargets(Colors::Black);

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();

	//モデルを回転
	static float rotation = 0.0F;
	rotation += static_cast<float>(XM_PI * 0.005F);
	if (rotation > 360.0F)
	{
		rotation -= 360;
	}

	world = XMMatrixRotationY(rotation);

	//モデルをレンダリング
	model_->render();

	if (!(ShaderManager::getInstance()->deferredRender(model_, world, view, projection, model_->getTexture())))
	{
		return false;
	}

	//バックバッファにレンダリングターゲットを移す
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//ビューポートをリセット
	Direct3D::getInstance()->resetViewPort();

	return true;
}

void Game::handleMovementInput()
{

	position_.setFrameTime(16.0F);

	position_.turnLeft(Input::getInstance()->keyDown(DIK_LEFT));
	position_.turnRight(Input::getInstance()->keyDown(DIK_RIGHT));
	position_.moveForWard(Input::getInstance()->keyDown(DIK_UP));
	position_.moveBackWard(Input::getInstance()->keyDown(DIK_DOWN));
	position_.moveUpWard(Input::getInstance()->keyDown(DIK_A));
	position_.moveDownWard(Input::getInstance()->keyDown(DIK_Z));
	position_.lookUpWard(Input::getInstance()->keyDown(DIK_PGUP));
	position_.lookDownWard(Input::getInstance()->keyDown(DIK_PGDN));
	
	camera_->setPosition(position_.getPosition());
	camera_->setRotation(position_.getRotation());
}
