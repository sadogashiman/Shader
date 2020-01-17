#pragma once
#include"Shadowshader.h"
#include"Maskshader.h"
#include"Bumpmap.h"
#include"Depthshader.h"
#include"Lightshader.h"
#include"Light.h"
#include"TextureShader.h"
#include"Deferredshader.h"

class Renderer
{
private:
	Renderer() = default;
	~Renderer() = default;
	std::unique_ptr<LightShader> lightshader_;
	std::unique_ptr<Bumpmap> bumpshader_;
	std::unique_ptr<Maskshader> maskshader_;
	std::unique_ptr<Shadowshader> shadowshader_;
	std::unique_ptr<Depthshader> depthshader_;
	std::unique_ptr<Textureshader>textureshader_;
	std::unique_ptr<Deferredshader>deferredshader_;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;
public:


	bool init();
	void destroy();

	static inline Renderer* getInstance()
	{
		static Renderer instance;
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
	bool shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture,ID3D11ShaderResourceView* Depthmaptexture2, Light* Lightdata,Light* Lightdata2);

	//テクスチャシェーダーを使用するときのレンダー(モデルのインデックスカウント、、各行列、テクスチャ)
	bool textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

	//遅延シェーディングを使用するときのレンダー
	bool deferredRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

};

