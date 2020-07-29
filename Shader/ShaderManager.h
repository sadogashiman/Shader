#pragma once

//シェーダー
#include"Shadowshader.h"
#include"Maskshader.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Light.h"
#include"TextureShader.h"
#include"Deferredshader.h"
#include"MultitexShader.h"
#include"Skydomeshader.h"
#include"Colorshader.h"
#include"SkyplaneShader.h"
#include"TerrainShader.h"
#include"ParticleShader.h"
#include"SpotlightShader.h"

//モデル
#include"Model3D.h"
#include"Model.h"
#include"SkyDome.h"
#include"Skyplane.h"
#include"Terrain.h"
#include"ParticleSystem.h"


class ShaderManager
{
private:
	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;
	ShaderManager(ShaderManager&&) = delete;
	ShaderManager& operator=(ShaderManager&&) = delete;

	std::unique_ptr<LightShader> lightshader_;
	std::unique_ptr<Maskshader> maskshader_;
	std::unique_ptr<Shadowshader> shadowshader_;
	std::unique_ptr<Depthshader> depthshader_;
	std::unique_ptr<Textureshader> textureshader_;
	std::unique_ptr<Deferredshader> deferredshader_;
	std::unique_ptr<MultitexShader> multitexshader_;
	std::unique_ptr<Skydomeshader> skydomeshader_;
	std::unique_ptr<Colorshader> colorshader_;
	std::unique_ptr<SkyplaneShader> skyplaneshader_;
	std::unique_ptr<TerrainShader> terrainshader_;
	std::unique_ptr<ParticleShader> particleshader_;
	std::unique_ptr<SpotlightShader> spotlightshader_;
public:

	bool init();

	static inline ShaderManager* getInstance()
	{
		static ShaderManager instance;
		return &instance;
	}

	//カラーシェーダーを使用するときのレンダー
	bool colorRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection,Vector4 Color);

	//マスクを使用するときのレンダー(モデルのポインタ、各行列、テクスチャ二枚とアルファマップの入ったテクスチャ配列)
	bool maskRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);

	//デプスマップを使用するときのレンダー(モデルのポインタ、各行列)
	bool depthRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection);
	bool depthRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection);

	//ライトシェーダーを使用するときのレンダー(モデルのポインタ、、各行列、テクスチャ、ライトのパラメーター)	※Deferredレンダリング前提
	bool lightRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* LightData);
	bool lightRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* LightData);

	//シャドウシェーダーを使用するときのレンダー(モデルのポインタ、、各行列、テクスチャ、デプスマップのテクスチャ)
	bool shadowRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);
	bool shadowRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);

	//スポットライトシェーダーを使用するときのレンダー
	bool spotRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Light* LightData);

	//テクスチャシェーダーを使用するときのレンダー(モデルのポインタ、、各行列、テクスチャ)
	bool textureRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//遅延シェーディングを使用するときのレンダー
	bool deferredRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//マルチテクスチャリングを使用するときのレンダー
	bool multiTextureRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum);

	//スカイドーム専用レンダー
	bool skyDomeRender(SkyDome* Skydome, Matrix World, Matrix View, Matrix Projection);

	//スカイドーム用雲専用レンダー
	bool skyPlaneRender(Skyplane* Skyplane, Matrix World, Matrix View, Matrix Projection);

	//テレイン専用レンダー
	bool terrainRender(Terrain* Model, Matrix World, Matrix View, Matrix Projection, Light* Light);

	//パーティクル専用シェーダー
	bool particleRender(ParticleSystem* ParticleSystem, Matrix World, Matrix View, Matrix Projection);
};

