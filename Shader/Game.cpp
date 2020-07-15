#include "stdafx.h"
#include "Game.h"
#include "Input.h"
#include "ShaderManager.h"
#include "System.h"
#include "Timer.h"
#include "SkyDome.h"
#include "Support.h"


Game::Game()
{
	instanceptr_ = Direct3D::getInstance();
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

	light_->setPosition(105.0F, 261.0F, 107.0F);
	light_->setAmbientColor(0.05F, 0.05F, 0.05F, 1.0F);
	light_->setDiffuseColor(0.8F, 0.8F, 0.8F, 1.0F);
	light_->setDirection(-0.5F, -1.0F, 0.0F);
	light_->generateProjection(kScreen_depth, kScreen_near);
	light_->generateOrthoMatrix(300.0F, kScreen_depth, kScreen_near);
	light_->setLookAt(128.0F, 0.0F, 128.0F);

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
	std::fstream fs;
	Vector3 position;
	Vector3 rotation;
	float scale;
	int cnt = 0;
	wchar_t modelpath[MAX_PATH];
	wchar_t texpath[MAX_PATH];
	std::string input;
	std::vector <std::thread> initthread;
	bool success = true;

	if (!Support::searchFile(L"Resource/Setup/citysetup.txt"))
	{
		Error::showDialog("セットアップファイルへのパスが無効です");
		return false;
	}

	auto a = sizeof(model_);

	//セットアップファイルを展開
	fs.open(L"Resource/Setup/citysetup.txt", std::ios::beg|std::ios::in);
	if (fs.fail())
	{
		return false;
	}

	//データ読み取り
	while (!fs.eof())
	{

		//モデル生成
		model_.push_back(new Model);

		//モデルファイル名を取得
		fs >> input;
		mbstowcs(modelpath, input.c_str(), MAX_PATH);

		//テクスチャファイル名を取得
		fs >> input;
		mbstowcs(texpath, input.c_str(), MAX_PATH);

		//モデルのパラメーターを取得
		fs >> position.x >> position.y >> position.z;
		fs >> rotation.x >> rotation.y >> rotation.z;
		fs >> scale;

		//取得したデータで初期化
		auto initialize = [&, cnt]()->void {success &= model_[cnt]->init(modelpath, texpath); };
		initthread.push_back(std::thread(initialize));

		model_[cnt]->setPosition(position);
		model_[cnt]->setRotation(rotation);
		model_[cnt]->setModelScale(scale);

		//添え字を加算
		cnt++;
	}

	//ファイルクローズ
	fs.close();

#ifdef _DEBUG
	//デバック時のみライト位置にモデルを生成する
	lightblock_ = new Model;
	result = lightblock_->init(L"Resource/Model/cube.txt", L"Resource/Texture/default.dds");
	if (!result)
	{
		return false;
	}
#endif // _DEBUG

	wire_ = false;


	//終了待ち
	for (auto& itr : initthread)
	{
		itr.join();
	}

	return success;
}

State* Game::update()
{
	bool result;

	//オブジェクト更新
	camera_->update();
	switchWireFrame();
	light_->update();

#ifdef _DEBUG
	lightblock_->setPosition(light_->getPosition());
#endif

	//レンダリング
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
	instanceptr_->begin(Colors::CornflowerBlue);

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
	instanceptr_->end();

	return true;
}

void Game::destroy()
{
	//破棄
	SAFE_DELETE_DESTROY(sky_);
	SAFE_DELETE_DESTROY(terrain_);
	SAFE_DELETE_DESTROY(cloud_);

	//生成したモデルを破棄
	for (auto itr :model_)
	{
		SAFE_DELETE_DESTROY(itr);
	}

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
	world = instanceptr_->getWorldMatrix();
	projection = instanceptr_->getProjectionMatrix();
	view = camera_->getViewMatrix();

	instanceptr_->turnCullingEnable();

	for (auto& itr : model_)
	{
		world = itr->getWorldMatrix();
		if (!ShaderManager::getInstance()->shadowRender(itr, world, view, projection, itr->getTexture(), rendertexture_->getShaderResouceView(), light_))
			return false;
	}

#ifdef _DEBUG
	world = lightblock_->getWorldMatrix();
	if (!ShaderManager::getInstance()->textureRender(lightblock_, world, view, projection, lightblock_->getTexture()))
		return false;
#endif // _DEBUG

	instanceptr_->turnCullingDisable();

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
	world = instanceptr_->getWorldMatrix();
	projection = instanceptr_->getProjectionMatrix();
	view = camera_->getViewMatrix();
	skyworld = XMMatrixTranslation(camera_->getPosition().x, camera_->getPosition().y, camera_->getPosition().z);

	//Zバッファ・カリングをオフ
	instanceptr_->turnCullingDisable();
	instanceptr_->turnZbufferDisable();

	//スカイドームをレンダリング
	if (!(ShaderManager::getInstance()->skyDomeRender(sky_, skyworld, view, projection)))
	{
		return false;
	}

	//カリングをオン
	instanceptr_->turnCullingEnable();

	//加算合成オン
	instanceptr_->turnAddBlendEnable();

	//雲をレンダリング
	if (!(ShaderManager::getInstance()->skyPlaneRender(cloud_, skyworld, view, projection)))
	{
		return false;
	}

	//合成オフ
	instanceptr_->turnAlphablendDisable();

	//Zバッファオン
	instanceptr_->turnZbufferEnable();

	//ワイヤーフレーム切り替え
	if (wire_)
		instanceptr_->wireFrameEnable();

	world = terrain_->getWorldMatrix();

	//テレインをレンダリング
	if (!(ShaderManager::getInstance()->terrainRender(terrain_, world, view, projection, light_)))
	{
		return false;
	}

	if (wire_)
		instanceptr_->wireFrameDisable();

	return true;
}

bool Game::renderToScene()
{
	Matrix world;
	Matrix view;
	Matrix projection;

	instanceptr_->setBackBufferRenderTarget();

	//描画先変更
	instanceptr_->turnCullingDisable();
	rendertexture_->setRenderTarget();
	rendertexture_->clearRenderTarget(Colors::ForestGreen);

	//射影行列を生成
	light_->generateView();

	world = instanceptr_->getWorldMatrix();

	//行列を取得
	view = light_->getViewMatrix();
	projection = light_->getProjectionMatrix();

	//深度マップにレンダリング
	world = terrain_->getWorldMatrix();
	terrain_->render();
	if (!ShaderManager::getInstance()->depthRender(terrain_->getIndexCount(), world, view, projection))
		return false;

	for (auto& itr : model_)
	{
		world = itr->getWorldMatrix();
		if (!ShaderManager::getInstance()->depthRender(itr, world, view, projection))
			return false;
	}

	//バックバッファにレンダーターゲットを移す
	instanceptr_->setBackBufferRenderTarget();

	//ビューポートをリセット
	instanceptr_->resetViewPort();

	return true;
}
