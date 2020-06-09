#pragma once

//シェーダー
#include"Shadowshader.h"
#include"Maskshader.h"
#include"Bumpmap.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Light.h"
#include"TextureShader.h"
#include"Deferredshader.h"
#include"Multitexture.h"
#include"Skydomeshader.h"

//モデル
#include"Model.h"
#include"SkyDome.h"


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
	std::unique_ptr<Bumpmap> bumpshader_;
	std::unique_ptr<Maskshader> maskshader_;
	std::unique_ptr<Shadowshader> shadowshader_;
	std::unique_ptr<Depthshader> depthshader_;
	std::unique_ptr<Textureshader>textureshader_;
	std::unique_ptr<Deferredshader>deferredshader_;
	std::unique_ptr<Multitexture>multitexshader_;
	std::unique_ptr<Skydomeshader> skydomeshader_;
public:

	bool init();

	static inline ShaderManager* getInstance()
	{
		static ShaderManager instance;
		return &instance;
	}

	//カラーシェーダーを使用するときのレンダー
	bool colorRender(Model* Model, Matrix World, Matrix View, Matrix Projection);

	//マスクを使用するときのレンダー(モデルのポインタ、各行列、テクスチャ二枚とアルファマップの入ったテクスチャ配列)
	bool maskRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);

	//バンプマップを使用するときのレンダー(モデルのポインタ、各行列、テクスチャ二枚と法線マップの入ったテクスチャ配列、ライトのパラメーター)
	bool bumpRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata);

	//デプスマップを使用するときのレンダー(モデルのポインタ、各行列)
	bool depthRender(Model* Model, Matrix World, Matrix View, Matrix Projection);

	//ライトシェーダーを使用するときのレンダー(モデルのポインタ、、各行列、テクスチャ、ライトのパラメーター)
	bool lightRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* LightData);
	bool lightRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* LightData);

	//シャドウシェーダーを使用するときのレンダー(モデルのポインタ、、各行列、テクスチャ、デプスマップのテクスチャ)
	bool shadowRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);

	//テクスチャシェーダーを使用するときのレンダー(モデルのポインタ、、各行列、テクスチャ)
	bool textureRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//遅延シェーディングを使用するときのレンダー
	bool deferredRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//マルチテクスチャリングを使用するときのレンダー
	bool multiTextureRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum);

	//スカイドーム専用レンダー
	bool skyDomeRender(SkyDome* Skydome, Matrix World, Matrix View, Matrix Projection);
};

