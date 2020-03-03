#pragma once
#include"Shadowshader.h"
#include"Maskshader.h"
#include"Bumpmap.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Light.h"
#include"TextureShader.h"
#include"Deferredshader.h"
#include"Multitexture.h"

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

public:

	bool init();
	void destroy();

	static inline ShaderManager* getInstance()
	{
		static ShaderManager instance;
		return &instance;
	}

	//マスクを使用するときのレンダー(モデルのインデックスカウント、各行列、テクスチャ二枚とアルファマップの入ったテクスチャ配列)
	bool maskRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);

	//バンプマップを使用するときのレンダー(モデルのインデックスカウント、各行列、テクスチャ二枚と法線マップの入ったテクスチャ配列、ライトのパラメーター)
	bool bumpRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata);

	//デプスマップを使用するときのレンダー(モデルのインデックスカウント、各行列)
	bool depthRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection);

	//ライトシェーダーを使用するときのレンダー(モデルのインデックスカウント、、各行列、テクスチャ、ライトのパラメーター)
	bool lightRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* Lightdata);

	//シャドウシェーダーを使用するときのレンダー(モデルのインデックスカウント、、各行列、テクスチャ、デプスマップのテクスチャ)
	bool shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata);

	//テクスチャシェーダーを使用するときのレンダー(モデルのインデックスカウント、、各行列、テクスチャ)
	bool textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//遅延シェーディングを使用するときのレンダー
	bool deferredRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//マルチテクスチャリングを使用するときのレンダー
	bool multiTextureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum);
};

