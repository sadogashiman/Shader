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

	result = terrain_->init(L"Resource/setup.txt", L"Resource/dirt.tga");
	if (!result)
	{
		return false;
	}

	orthowindow_ = new OrthoWindow;
	if (!orthowindow_)
	{
		return false;
	}

	result = orthowindow_->init(kWindow_Width, kWindow_Height);
	if (!result)
	{
		Error::showDialog("2Dウィンドウの初期化に失敗");
		return false;
	}

	Timer::getInstance()->setTimerStatus(true);
	Timer::getInstance()->startTimer();

	position_.setPosition(Vector3(128.0F, 10.0F, -10.0F));
	position_.setRotation(Vector3::Zero);

	wire_ = false;

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
	Matrix world, projection,ortho;
	Matrix baseview;
	result = renderSceneToTexture();
	if (!result)
	{
		return false;
	}

	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::CornflowerBlue);

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	baseview = camera_->getBaseViewMatrix();
	ortho = Direct3D::getInstance()->getOrtho();
	
	//レンダリングは2Dで行うのでZバッファを無効にする
	Direct3D::getInstance()->turnZbufferOff();

	//描画用の2Dウィンドウを準備
	orthowindow_->render();

	result = ShaderManager::getInstance()->lightRender(orthowindow_->getIndexCount(), world, baseview, ortho,defbuffer_->getShaderResourceView(0),defbuffer_->getShaderResourceView(1),light_);
	if (!result)
	{
		return false;
	}

	//2Dレンダリングが終了したのでZバッファを有効にする
	Direct3D::getInstance()->turnZbufferOn();



	//描画終了
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE_DESTROY(orthowindow_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Game::renderSceneToTexture()
{
	Matrix world, view, projection;
	Matrix skyworld;

	//レンダーターゲットを作成
	defbuffer_->setRenderTargets();

	//レンダーバッファをクリア
	defbuffer_->clearRenderTargets(Colors::Black);

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	view = camera_->getViewMatrix();
	skyworld = XMMatrixTranslation(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	camera_->render();

	Direct3D::getInstance()->turnCullingOff();
	Direct3D::getInstance()->turnZbufferOff();

	if (!(ShaderManager::getInstance()->skyDomeRender(sky_, skyworld, view, projection)))
	{
		return false;
	}

	Direct3D::getInstance()->turnZbufferOn();
	Direct3D::getInstance()->turnCullingOn();

	if (wire_)
		Direct3D::getInstance()->wireFrameEnable();

	if (!(ShaderManager::getInstance()->deferredRender(terrain_, world, view, projection, terrain_->getTexture())))
	{
		return false;
	}

	if (wire_)
		Direct3D::getInstance()->wireFrameDisable();

	//バックバッファにレンダリングターゲットを移す
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//ビューポートをリセット
	Direct3D::getInstance()->resetViewPort();

	return true;
}

void Game::handleMovementInput()
{
	//カメラ制御
	position_.setFrameTime(16.0F);

	position_.turnLeft(Input::getInstance()->isKeyState(DIK_LEFT));
	position_.turnRight(Input::getInstance()->isKeyState(DIK_RIGHT));
	position_.moveForWard(Input::getInstance()->isKeyState(DIK_UP));
	position_.moveBackWard(Input::getInstance()->isKeyState(DIK_DOWN));
	position_.moveUpWard(Input::getInstance()->isKeyState(DIK_A));
	position_.moveDownWard(Input::getInstance()->isKeyState(DIK_Z));
	position_.lookUpWard(Input::getInstance()->isKeyState(DIK_PGUP));
	position_.lookDownWard(Input::getInstance()->isKeyState(DIK_PGDN));
	
	camera_->setPosition(position_.getPosition());
	camera_->setRotation(position_.getRotation());
	Input::getInstance()->anyKeyUp();
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
