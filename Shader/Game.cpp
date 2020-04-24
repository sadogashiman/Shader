#include "stdafx.h"
#include "Game.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"ShaderManager.h"

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

	result = model_->init(L"Resource/cube.txt");
	if (!result)
	{
		return false;
	}

	//全画面の2Dウィンドウを生成
	ortho_ = new OrthoWindow;
	if (!ortho_)
	{
		return false;
	}

	result = ortho_->init(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
	if (!result)
	{
		return false;
	}

	//バッファを作成
	defbuffer_ = new Deferredbuffers;
	if (!defbuffer_)
	{
		return false;
	}

	if (!defbuffer_->init(ScreenWidth, ScreenHeight, kScreen_depth, kScreen_near))
	{
		return false;
	}

	raytrace_ = new RayTrace;
	Sphere sp;
	sp.position = Vector3::Zero;
	sp.rdius = 0.4F;

	raytrace_->addSphere(&sp);

	rayhw_ = new Ray_trace_HW;
	if (!rayhw_->init())
	{
		return false;
	}


	return true;

}

bool Game::update()
{
	bool result;
	result = rayhw_->render();
	//result = raytrace_->render();

	//result = render();
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
	static float rotation = 0;

	if (rotation > 360.0F)
	{
		rotation -= 360.0F;
	}
	else
	{
		rotation++;
	}


	////シーンをレンダーバッファーにレンダリング
	//result = renderSceneToTexture();
	//if (!result)
	//{
	//	return false;
	//}

	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::Black);

	//行列を取得
	world = Direct3D::getInstance()->getWorld();
	projection = Direct3D::getInstance()->getProjection();
	ortho = Direct3D::getInstance()->getOrtho();
	view = camera_->getBaseViewMatrix();
	world = XMMatrixRotationY(XMConvertToRadians(rotation));

	//Direct3D::getInstance()->turnZbufferOff();

	//フルスクリーンの2Dウィンドウを作成
	//ortho_->render();
	if (!(ShaderManager::getInstance()->textureRender(model_, world, view, projection,model_->getTexture())))
	{
		return false;
	}

	//if (!(ShaderManager::getInstance()->lightRender(ortho_->getIndexCount(),world,view,ortho,defbuffer_->getShaderResourceView(0),defbuffer_->getShaderResourceView(1),light_)))
	//{
	//	return false;
	//}

	////すべての2Dレンダリングが終了したのでZバッファを有効にする
	//Direct3D::getInstance()->turnZbufferOn();

	//描画終了
	Direct3D::getInstance()->end();

	return true;
}

void Game::destroy()
{
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(ortho_);
	SAFE_DELETE_DESTROY(model_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
	rayhw_->destroy();

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