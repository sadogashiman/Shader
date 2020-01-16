#include "stdafx.h"
#include "Renderer.h"
#include"Direct3D.h"

bool Renderer::init()
{
	////各シェーダーオブジェクトを生成・初期化
	//lightshader_.reset(new LightShader);
	//if (!lightshader_.get())
	//{
	//	return false;
	//}

	//if (!(lightshader_.get()->init()))
	//{
	//	Error::showDialog("ライトシェーダーの初期化に失敗");
	//	return false;
	//}

	//bumpshader_.reset(new Bumpmap);
	//if (!bumpshader_.get())
	//{
	//	return false;
	//}

	//if (!(bumpshader_.get()->init()))
	//{
	//	Error::showDialog("バンプシェーダーの初期化に失敗");

	//	return false;
	//}

	//alphashader_.reset(new Alphamapshader);
	//if (!alphashader_.get())
	//{
	//	return false;
	//}

	//if (!(alphashader_.get()->init()))
	//{
	//	Error::showDialog("アルファシェーダーの初期化に失敗");

	//	return false;
	//}

	//shadowshader_.reset(new Shadowshader);
	//if (!shadowshader_.get())
	//{
	//	return false;
	//}

	//if (!(shadowshader_.get()->init()))
	//{
	//	Error::showDialog("シャドウシェーダーの初期化に失敗");
	//	return false;
	//}

	//depthshader_.reset(new Depthshader);
	//if (!depthshader_.get())
	//{
	//	return false;
	//}

	//if (!(depthshader_.get()->init()))
	//{
	//	Error::showDialog("デプスシェーダーの初期化に失敗");
	//	return false;
	//}

	textureshader_.reset(new Textureshader);
	if (!textureshader_.get())
	{
		return false;
	}

	if (!(textureshader_.get()->init()))
	{
		Error::showDialog("テクスチャシェーダーの初期化に失敗");
		return false;
	}

	//deferredbuffer_.reset(new Deferredbuffers);
	//if (!deferredbuffer_.get())
	//{
	//	return false;
	//}

	//if (!(deferredbuffer_.get()->init(1280,720,100.F,1.0F)))
	//{

	//	Error::showDialog("遅延バッファの初期化に失敗");
	//	return false;
	//}

	return true;
}

void Renderer::destroy()
{
	//lightshader_.get()->destroy();
	//bumpshader_.get()->destroy();
	//alphashader_.get()->destroy();
	//shadowshader_.get()->destroy();
	//depthshader_.get()->destroy();
	//deferredbuffer_.get()->destroy();
	textureshader_.get()->destroy();
}

bool Renderer::maskRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	bool result;

	result = alphashader_.get()->render(Indexcount, World, View, Projection, TextureArray);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Renderer::bumpRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata)
{
	bool result;

	result = bumpshader_.get()->render(Indexcount, World, View, Projection, TextureArray,Lightdata->getDirection(),Lightdata->getDiffuseColor());
	if (!result)
	{
		return false;
	}

	return true;
}

bool Renderer::depthRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection)
{
	bool result;

	result = depthshader_.get()->render(Indexcount, World, View, Projection);
	if (!result)
	{
		return false;
	}
	return true;
}

bool Renderer::lightRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* Lightdata)
{
	bool result;

	result = lightshader_.get()->render(Indexcount, World, View, Projection,Texture1,Texture2, Lightdata->getDirection());
	if (!result)
	{
		return false;
	}

	return true;
}

bool Renderer::shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, ID3D11ShaderResourceView* Depthmaptexture2, Light* Lightdata,Light* Lightdata2)
{
	bool result;

	result = shadowshader_.get()->render(Indexcount, World, View, Projection, Lightdata->getViewMatrix(), Lightdata->getProjection(), Texture, Depthmaptexture, Lightdata->getPosition(), Lightdata->getAmbientColor(), Lightdata->getDiffuseColor());
	if (!result)
	{
		return false;
	}

	return true;
}

bool Renderer::textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	bool result;

	result = textureshader_.get()->render(Indexcount, World, View, Projection, Texture);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Renderer::deferredRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	return true;
}
