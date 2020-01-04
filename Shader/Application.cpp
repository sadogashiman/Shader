#include "stdafx.h"
#include "Application.h"
#include"Direct3D.h"
#include"release.h"
#include"Input.h"
#include"Renderer.h"

Application::Application()
{
	light_ = nullptr;
	lightshader_ = nullptr;
	model_ = nullptr;
	camera_ = nullptr;
	ortho_ = nullptr;
	defbuffer_ = nullptr;
	defshader_ = nullptr;
}

Application::~Application()
{
}

bool Application::init(HWND Hwnd, const int ScreenWidth, const int ScreenHeight)
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

	result = model_->init(L"Resource/seafloor.dds",L"Resource/cube.txt");
	if (!result)
	{
		return false;
	}

	//全画面のオルソウィンドウを生成
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

	//遅延バッファオブジェクトを生成
	defbuffer_ = new Deferredbuffers;
	if (!defbuffer_)
	{
		return false;
	}

	//遅延バッファオブジェクトを初期化
	result = defbuffer_->init(ScreenWidth, ScreenHeight, kScreen_depth, kScreen_near);
	if (!result)
	{
		return false;
	}
	defshader_ = new Deferredshader;
	if (!defshader_)
	{
		return false;
	}

	//遅延シェーダーオブジェクトを生成
	result = defshader_->init();
	if (!result)
	{
		return false;
	}

	//ライトシェーダーオブジェクトを生成
	lightshader_ = new LightShader;
	if (!result)
	{
		return false;
	}

	result = lightshader_->init();
	if (!result)
	{
		return false;
	}

	return true;
}

bool Application::update()
{
	return true;
}

bool Application::render()
{
	bool result;
	Matrix world, baseview, ortho;

	//シーンをレンダーバッファーにレンダリング
	result = renderSceneToTexture();
	if (!result)
	{
		return false;
	}

	//シーンをクリア
	Direct3D::getInstance()->begin(Colors::Black);

	//行列を取得
	Direct3D::getInstance()->getWorld(world);
	Direct3D::getInstance()->getOrtho(ortho);
	baseview = camera_->getBaseViewMatrix();

	//2Dレンダリング開始
	Direct3D::getInstance()->turnZbufferOff();

	ortho_->render();

	//フルスクリーンの2Dウィンドウを作成
	Renderer::getInstance()->lightRender(ortho_->getIndexCount(), world, baseview, ortho, defbuffer_->getShaderResourceView(0), defbuffer_->getShaderResourceView(1), light_);

	//すべての2Dレンダリングが終了したのでZバッファを有効にする
	Direct3D::getInstance()->turnZbufferOn();

	//描画終了
	Direct3D::getInstance()->end();

	return true;
}

void Application::destroy()
{
	SAFE_DELETE_DESTROY(lightshader_);
	SAFE_DELETE_DESTROY(defshader_);
	SAFE_DELETE_DESTROY(defbuffer_);
	SAFE_DELETE_DESTROY(ortho_);
	SAFE_DELETE_DESTROY(model_);
	SAFE_DELETE(light_);
	SAFE_DELETE(camera_);
}

bool Application::renderSceneToTexture()
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

	defshader_->render(model_->getIndexCount(), world, view, projection, model_->getTexture());

	//バックバッファにレンダリングターゲットを移す
	Direct3D::getInstance()->setBackBufferRenderTarget();

	//ビューポートをリセット
	Direct3D::getInstance()->resetViewPort();

	return true;
}