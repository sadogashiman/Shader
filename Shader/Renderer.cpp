#include "stdafx.h"
#include "Renderer.h"
#include"Direct3D.h"

bool Renderer::init()
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

	//shadowshader_.reset(new Shadowshader);
	//if (!shadowshader_.get())
	//{
	//	return false;
	//}

	//if (!(shadowshader_.get()->init()))
	//{
	//	Error::showDialog("�V���h�E�V�F�[�_�[�̏������Ɏ��s");
	//	return false;
	//}

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

void Renderer::destroy()
{
	lightshader_.get()->destroy();
	bumpshader_.get()->destroy();
	maskshader_.get()->destroy();
	//shadowshader_.get()->destroy();
	depthshader_.get()->destroy();
	textureshader_.get()->destroy();
	multitexshader_.get()->destroy();
}

bool Renderer::maskRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	bool result;

	result = maskshader_.get()->render(Indexcount, World, View, Projection, TextureArray);
	if (!result)
	{
		Error::showDialog("�����_�����O�Ɏ��s(�}�X�N�V�F�[�_�[)");
		return false;
	}

	return true;
}

bool Renderer::bumpRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Light* Lightdata)
{
	bool result;

	result = bumpshader_.get()->render(Indexcount, World, View, Projection, TextureArray, Lightdata->getDirection(), Lightdata->getDiffuseColor());
	if (!result)
	{
		Error::showDialog("�����_�����O�Ɏ��s(�o���v�V�F�[�_�[)");
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
		Error::showDialog("�����_�����O�Ɏ��s(�f�v�X�V�F�[�_�[)");
		return false;
	}
	return true;
}

bool Renderer::lightRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, Light* Lightdata)
{
	bool result;

	result = lightshader_.get()->render(Indexcount, World, View, Projection, Texture1, Texture2, Lightdata->getDirection());
	if (!result)
	{
		Error::showDialog("�����_�����O�Ɏ��s(���C�g�V�F�[�_�[)");
		return false;
	}

	return true;
}

bool Renderer::shadowRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, ID3D11ShaderResourceView* Depthmaptexture2, Light* Lightdata, Light* Lightdata2)
{
	bool result;

	result = shadowshader_.get()->render(Indexcount, World, View, Projection, Lightdata->getViewMatrix(), Lightdata->getProjection(), Texture, Depthmaptexture, Lightdata->getPosition(), Lightdata->getAmbientColor(), Lightdata->getDiffuseColor());
	if (!result)
	{
		Error::showDialog("�����_�����O�Ɏ��s(�V���h�E�V�F�[�_�[)");
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
		Error::showDialog("�����_�����O�Ɏ��s(�e�N�X�`���V�F�[�_�[)");
		return false;
	}

	return true;
}

bool Renderer::deferredRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	bool result;

	result = deferredshader_.get()->render(Indexcount, World, View, Projection, Texture);
	if (!result)
	{
		Error::showDialog("�����_�����O�Ɏ��s(�f�B�t�@�[�h�V�F�[�_�[)");
		return false;
	}
	return true;
}

bool Renderer::multiTextureRender(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, const int Texturenum)
{
	bool result;

	result = multitexshader_.get()->render(Indexcount, World, View, Projection, TextureArray, Texturenum);
	if (!result)
	{
		Error::showDialog("�����_�����O�Ɏ��s(�}���`�e�N�X�`���V�F�[�_�[");
		return false;
	}

	return true;
}
