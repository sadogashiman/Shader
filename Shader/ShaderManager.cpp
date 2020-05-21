#include "stdafx.h"
#include "ShaderManager.h"
#include"Direct3D.h"

bool ShaderManager::init()
{
	//各シェーダーオブジェクトを生成・初期化
	lightshader_.reset(new LightShader);
	if (!lightshader_.get())
	{
		return false;
	}

	if (!(lightshader_.get()->init()))
	{
		Error::showDialog("ライトシェーダーの初期化に失敗");
		return false;
	}

	bumpshader_.reset(new Bumpmap);
	if (!bumpshader_.get())
	{
		return false;
	}

	if (!(bumpshader_.get()->init()))
	{
		Error::showDialog("バンプシェーダーの初期化に失敗");

		return false;
	}
	maskshader_.reset(new Maskshader);
	if (!maskshader_.get())
	{
		return false;
	}

	if (!(maskshader_.get()->init()))
	{
		Error::showDialog("アルファシェーダーの初期化に失敗");

		return false;
	}

	shadowshader_.reset(new Shadowshader);
	if (!shadowshader_.get())
	{
		return false;
	}

	if (!(shadowshader_.get()->init()))
	{
		Error::showDialog("シャドウシェーダーの初期化に失敗");
		return false;
	}

	depthshader_.reset(new Depthshader);
	if (!depthshader_.get())
	{
		return false;
	}

	if (!(depthshader_.get()->init()))
	{
		Error::showDialog("デプスシェーダーの初期化に失敗");
		return false;
	}

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

	deferredshader_.reset(new Deferredshader);
	if (!deferredshader_.get())
	{
		return false;
	}

	if (!(deferredshader_.get()->init()))
	{
		Error::showDialog("ディファードシェーダーの初期化に失敗");
		return false;
	}


	multitexshader_.reset(new Multitexture);
	if (!multitexshader_.get())
	{
		return false;
	}

	if (!(multitexshader_.get()->init()))
	{
		Error::showDialog("マルチテクスチャシェーダーの初期化に失敗");
		return false;
	}
	
	return true;
}

void ShaderManager::destroy()
{
}

bool ShaderManager::maskRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	Model->render();
	return maskshader_.get()->render(Model->getIndexCount(), World, View, Projection, TextureArray);
}

bool ShaderManager::bumpRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata)
{
	Model->render();
	return bumpshader_.get()->render(Model->getIndexCount(), World, View, Projection, TextureArray, Lightdata->getDirection(), Lightdata->getDiffuseColor());
}

bool ShaderManager::depthRender(Model* Model, Matrix World, Matrix View, Matrix Projection)
{
	Model->render();
	return depthshader_.get()->render(Model->getIndexCount(), World, View, Projection);
}

bool ShaderManager::lightRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* Lightdata)
{
	Model->render();
	return lightshader_.get()->render(Model->getIndexCount(), World, View, Projection, Texture1, Texture2, Lightdata->getDirection());
}

bool ShaderManager::shadowRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata)
{
	Model->render();
	return shadowshader_.get()->render(Model->getIndexCount(), World, View, Projection, Lightdata->getViewMatrix(), Lightdata->getProjection(), Texture, Depthmaptexture, Lightdata->getPosition(), Lightdata->getAmbientColor(), Lightdata->getDiffuseColor());
}

bool ShaderManager::textureRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	Model->render();
	return textureshader_.get()->render(Model->getIndexCount(), World, View, Projection, Texture);
}

bool ShaderManager::deferredRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	Model->render();
	return deferredshader_.get()->render(Model->getIndexCount(), World, View, Projection, Texture);
}

bool ShaderManager::multiTextureRender(Model* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum)
{
	Model->render();
	return multitexshader_.get()->render(Model->getIndexCount(), World, View, Projection, TextureArray, Texturenum);
}
