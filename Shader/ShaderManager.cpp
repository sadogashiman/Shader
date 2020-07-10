#include "stdafx.h"
#include "ShaderManager.h"

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

	skydomeshader_.reset(new Skydomeshader);
	if (!skydomeshader_.get())
	{
		return false;
	}

	if (!(skydomeshader_.get()->init()))
	{
		Error::showDialog("スカイドームシェーダーの初期化に失敗");
		return false;
	}

	colorshader_.reset(new Colorshader);
	if (!colorshader_.get())
	{
		return false;
	}

	if (!colorshader_.get()->init())
	{
		Error::showDialog("カラーシェーダーの初期化に失敗");
		return false;
	}

	skyplaneshader_.reset(new SkyplaneShader);
	if (!skyplaneshader_.get())
	{
		return false;
	}

	if (!skyplaneshader_.get()->init())
	{
		Error::showDialog("SkyPlaneシェーダーの初期化に失敗");
		return false;
	}

	terrainshader_.reset(new TerrainShader);
	if (!terrainshader_.get())
	{
		return false;
	}

	if (!terrainshader_.get()->init())
	{
		Error::showDialog("テレインシェーダーの初期化に失敗");
		return false;
	}

	particleshader_.reset(new ParticleShader);
	if (!particleshader_.get())
	{
		return false;
	}

	if (!particleshader_.get()->init())
	{
		Error::showDialog("パーティクルシェーダーの初期化に失敗");
		return false;
	}

	return true;
}

bool ShaderManager::colorRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection)
{
	Model->render();
	return colorshader_.get()->render(Model->getIndexCount(), World, View, Projection);
}

bool ShaderManager::maskRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	Model->render();
	return maskshader_.get()->render(Model->getIndexCount(), World, View, Projection, TextureArray);
}

bool ShaderManager::depthRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection)
{
	Model->render();
	return depthshader_.get()->render(Model->getIndexCount(), World, View, Projection);
}

bool ShaderManager::depthRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection)
{
	return depthshader_.get()->render(IndexCount, World, View, Projection);
}

bool ShaderManager::lightRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* LightData)
{
	Model->render();
	return lightshader_.get()->render(Model->getIndexCount(), World, View, Projection, Texture1, Texture2, LightData->getDirection());
}

bool ShaderManager::lightRender(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* LightData)
{
	return lightshader_.get()->render(IndexCount, World, View, Projection, Texture1, Texture2, LightData->getDirection());
}

bool ShaderManager::shadowRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata)
{
	Model->render();
	return shadowshader_.get()->render(Model->getIndexCount(), World, View, Projection, Lightdata->getViewMatrix(), Lightdata->getOrthoMatrix(), Texture, Depthmaptexture, Lightdata->getDirection(), Lightdata->getAmbientColor(), Lightdata->getDiffuseColor());
}

bool ShaderManager::textureRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	Model->render();
	return textureshader_.get()->render(Model->getIndexCount(), World, View, Projection, Texture);
}

bool ShaderManager::deferredRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	Model->render();
	return deferredshader_.get()->render(Model->getIndexCount(), World, View, Projection, Texture);
}

bool ShaderManager::multiTextureRender(Model3D* Model, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum)
{
	Model->render();
	return multitexshader_.get()->render(Model->getIndexCount(), World, View, Projection, TextureArray, Texturenum);
}

bool ShaderManager::skyDomeRender(SkyDome* Skydome, Matrix World, Matrix View, Matrix Projection)
{
	Skydome->render();
	return skydomeshader_.get()->render(Skydome->getIndexCount(),World,View,Projection,Skydome->getApexColor(),Skydome->getCenterColor());
}

bool ShaderManager::skyPlaneRender(Skyplane* Skyplane, Matrix World, Matrix View, Matrix Projection)
{
	Skyplane->render();
	return skyplaneshader_.get()->render(Skyplane->getIndexCount(), World, View, Projection, Skyplane->getCloudTexture(), Skyplane->getPerturbTexture(),Skyplane->getTransition(),Skyplane->getScale(),Skyplane->getBrightness());
}

bool ShaderManager::terrainRender(Terrain* Model, Matrix World, Matrix View, Matrix Projection, Light* Light)
{
	Model->render();
	return terrainshader_.get()->render(Model->getIndexCount(), World, View, Projection, Light->getAmbientColor(), Light->getDiffuseColor(), Light->getDirection(),Model->getTexture());
}

bool ShaderManager::particleRender(ParticleSystem* ParticleSystem, Matrix World, Matrix View, Matrix Projection)
{
	ParticleSystem->render();
	return particleshader_.get()->render(ParticleSystem->getIndexCount(), World, View, Projection, ParticleSystem->getTexture());
}
