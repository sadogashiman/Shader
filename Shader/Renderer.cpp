#include "stdafx.h"
#include "Renderer.h"
#include"Direct3D.h"

bool Renderer::init()
{
	//各シェーダーオブジェクトを生成・初期化
	lightshader_.reset(new LightShader);
	if (!lightshader_.get())
	{
		return false;
	}

	if (!(lightshader_.get()->init()))
	{
		return false;
	}

	bumpshader_.reset(new Bumpmap);
	if (!bumpshader_.get())
	{
		return false;
	}

	if (!(bumpshader_.get()->init()))
	{
		return false;
	}

	alphashader_.reset(new Alphamapshader);
	if (!alphashader_.get())
	{
		return false;
	}

	if (!(alphashader_.get()->init()))
	{
		return false;
	}

	shadowshader_.reset(new Shadowshader);
	if (!shadowshader_.get())
	{
		return false;
	}

	if (!(shadowshader_.get()->init()))
	{
		return false;
	}

	depthshader_.reset(new Depthshader);
	if (!depthshader_.get())
	{
		return false;
	}

	if (!(depthshader_.get()->init()))
	{
		return false;
	}

	deferredbuffer_.reset(new Deferredbuffers);
	if (!deferredbuffer_.get())
	{
		return false;
	}

	if (!(deferredbuffer_.get()->init(1280,720,100.F,1.0F)))
	{
		return false;
	}

	return true;
}

void Renderer::destroy()
{
}

bool Renderer::maskRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	return true;
}

bool Renderer::bumpRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata)
{
	return true;
}

bool Renderer::depthRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection)
{
	return true;
}

bool Renderer::lightRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2, Light* Lightdata)
{
	bool result;

	result = lightshader_.get()->render(Indexcount, World, View, Projection,Texture1,Texture2, Lightdata->GetDirection());
	if (!result)
	{
		return false;
	}

	return true;
}

bool Renderer::shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture)
{
	return true;
}

bool Renderer::textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	return true;
}

bool Renderer::multiTexRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	return true;
}
