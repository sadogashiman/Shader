#include "stdafx.h"
#include "ShaderManager.h"
#include"Direct3D.h"

bool ShaderManager::init()
{
	//�e�V�F�[�_�[�I�u�W�F�N�g�𐶐��E������
	lightshader_.reset(new LightShader);
	if (!lightshader_.get())
	{
		return false;
	}

	if (!(lightshader_.get()->init()))
	{
		Error::showDialog("���C�g�V�F�[�_�[�̏������Ɏ��s");
		return false;
	}

	bumpshader_.reset(new Bumpmap);
	if (!bumpshader_.get())
	{
		return false;
	}

	if (!(bumpshader_.get()->init()))
	{
		Error::showDialog("�o���v�V�F�[�_�[�̏������Ɏ��s");

		return false;
	}

	maskshader_.reset(new Maskshader);
	if (!maskshader_.get())
	{
		return false;
	}

	if (!(maskshader_.get()->init()))
	{
		Error::showDialog("�A���t�@�V�F�[�_�[�̏������Ɏ��s");

		return false;
	}

	shadowshader_.reset(new Shadowshader);
	if (!shadowshader_.get())
	{
		return false;
	}

	if (!(shadowshader_.get()->init()))
	{
		Error::showDialog("�V���h�E�V�F�[�_�[�̏������Ɏ��s");
		return false;
	}

	depthshader_.reset(new Depthshader);
	if (!depthshader_.get())
	{
		return false;
	}

	if (!(depthshader_.get()->init()))
	{
		Error::showDialog("�f�v�X�V�F�[�_�[�̏������Ɏ��s");
		return false;
	}

	textureshader_.reset(new Textureshader);
	if (!textureshader_.get())
	{
		return false;
	}

	if (!(textureshader_.get()->init()))
	{
		Error::showDialog("�e�N�X�`���V�F�[�_�[�̏������Ɏ��s");
		return false;
	}

	deferredshader_.reset(new Deferredshader);
	if (!deferredshader_.get())
	{
		return false;
	}

	if (!(deferredshader_.get()->init()))
	{
		Error::showDialog("�f�B�t�@�[�h�V�F�[�_�[�̏������Ɏ��s");
		return false;
	}

	multitexshader_.reset(new Multitexture);
	if (!multitexshader_.get())
	{
		return false;
	}

	if (!(multitexshader_.get()->init()))
	{
		Error::showDialog("�}���`�e�N�X�`���V�F�[�_�[�̏������Ɏ��s");
		return false;
	}

	return true;
}

void ShaderManager::destroy()
{
	textureshader_.get()->destroy();
	multitexshader_.get()->destroy();
	deferredshader_.get()->destroy();
	depthshader_.get()->destroy();
	shadowshader_.get()->destroy();
	maskshader_.get()->destroy();
	bumpshader_.get()->destroy();
	lightshader_.get()->destroy();
}

bool ShaderManager::maskRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	return maskshader_.get()->render(Indexcount, World, View, Projection, TextureArray);
}

bool ShaderManager::bumpRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata)
{
	return bumpshader_.get()->render(Indexcount, World, View, Projection, TextureArray, Lightdata->getDirection(), Lightdata->getDiffuseColor());
}

bool ShaderManager::depthRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection)
{
	return depthshader_.get()->render(Indexcount, World, View, Projection);
}

bool ShaderManager::lightRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* Lightdata)
{
	return lightshader_.get()->render(Indexcount, World, View, Projection, Texture1, Texture2, Lightdata->getDirection());
}

bool ShaderManager::shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Light* Lightdata)
{
	return shadowshader_.get()->render(Indexcount, World, View, Projection, Lightdata->getViewMatrix(), Lightdata->getProjection(), Texture, Depthmaptexture, Lightdata->getPosition(), Lightdata->getAmbientColor(), Lightdata->getDiffuseColor());
}

bool ShaderManager::textureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	return textureshader_.get()->render(Indexcount, World, View, Projection, Texture);
}

bool ShaderManager::deferredRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	return deferredshader_.get()->render(Indexcount, World, View, Projection, Texture);
}

bool ShaderManager::multiTextureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum)
{
	return multitexshader_.get()->render(Indexcount, World, View, Projection, TextureArray, Texturenum);
}
