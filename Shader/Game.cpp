#include "stdafx.h"
#include "Game.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"Renderer.h"

Game::Game()
{
	light_ = nullptr;
	lightshader_ = nullptr;
	model_ = nullptr;
	camera_ = nullptr;
	ortho_ = nullptr;
	defbuffer_ = nullptr;
}

Game::~Game()
{
}

bool Game::init(HWND Hwnd, const int ScreenWidth, const int ScreenHeight)
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

	//モデルオブジェクトを生成
	model_ = new Model;
	if (!model_)
	{
		return false;
	}

	result = model_->init(L"Resource/stone01.dds",L"Resource/model.txt",kBumpMap);
	if (!result)
	{
		return false;
	}

	//全画面の2Dウィンドウを生成
	//ortho_ = new OrthoWindow;
	//if (!ortho_)
	//{
	//	return false;
	//}

	//result = ortho_->init(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
	//if (!result)
	//{
	//	return false;
	//}

	//バッファを作成
	//defbuffer_ = new Deferredbuffers;
	//if (!defbuffer_)
	//{
	//	return false;
	//}

	//if (!defbuffer_->init(ScreenWidth, ScreenHeight, kScreen_depth, kScreen_near))
	//{
	//	return false;
	//}

	return true;
}

bool Game::update()
{
	bool result;
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Game::render()
{
	bool result;
	Matrix world, view, projection,ortho;

	////シーンをレンダーバッファーにレンダリング
	//result = renderSceneToTexture();
	//if (!result)
	//{
	//	return false;
	//}

	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::Black);

	//行列を取得
	Direct3D::getInstance()->getWorld(world);
	Direct3D::getInstance()->getProjection(projection);
	Direct3D::getInstance()->getOrtho(ortho);
	view = camera_->getBaseViewMatrix();

	//Direct3D::getInstance()->turnZbufferOff();

	//フルスクリーンの2Dウィンドウを作成
	//ortho_->render();

		//モデルを回転
	static float rotation = 0.0F;
	rotation += static_cast<float>(XM_PI * 0.005F);
	if (rotation > 360.0F)
	{
		rotation -= 360;
	}

	world = XMMatrixRotationY(rotation);

	model_->render();

	if (!(Renderer::getInstance()->textureRender(model_->getIndexCount(),world,view,projection,model_->getTexture())))
	{
		return false;
	}

	//すべての2Dレンダリングが終了したのでZバッファを有効にする
	//Direct3D::getInstance()->turnZbufferOn();

	//描画終了
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	//SAFE_DELETE_DESTROY(defbuffer_);
	//SAFE_DELETE_DESTROY(ortho_);
	SAFE_DELETE_DESTROY(model_);
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
	Direct3D::getInstance()->getWorld(world);
	Direct3D::getInstance()->getProjection(projection);
	view = camera_->getViewMatrix();



	//モデルをレンダリング
	model_->render();

	if (!(Renderer::getInstance()->deferredRender(model_->getIndexCount(), world, view, projection, model_->getTexture())))
	{
		return false;
	}

	//バックバッファにレンダリングターゲットを移す
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//ビューポートをリセット
	Direct3D::getInstance()->resetViewPort();

	return true;
}