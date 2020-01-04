#include "stdafx.h"
#include "Texture.h"
#include"release.h"
#include"Direct3D.h"
#include"error.h"

Texture::Texture()
{
	texture_ = nullptr;
	textureresource_ = nullptr;
}

Texture::~Texture()
{
}

bool Texture::init(const wchar_t* TextureName)
{
	HRESULT hr;

	//�ǂ̃��[�_�[���g�p���邩�g���q�Ŕ��f
	std::filesystem::path texturepath = TextureName;

	//�n���ꂽ�t�@�C�����Ɋg���q���܂܂�Ă��邩�m�F
	if (texturepath.has_extension())
	{
		//�g���q���e�N�X�`�����[�_�[�ɑΉ����Ă��邩�m�F
		if (!checkExtension(texturepath.extension()))
		{
			Error::showDialog("�e�N�X�`�����[�_�[���Ή����Ă��Ȃ��g���q�ł�");
			return false;
		}
		
		//�g���q��dds����DDS�̏ꍇDDS���[�_�[���g�p
		if(wcscmp(extensiontype[kDds],TextureName)==0)
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName, NULL, &texture_, NULL);
			if (FAILED(hr))
			{
				Error::showDialog("DDSTextureLoader�ł̓ǂݍ��݂Ɏ��s���܂���");
				return false;
			}
		}

		//WICTextureLoader�œǂ߂���̂�WIC���g�p
		for (int i = kPng; i = kGif; i++)
		{
			if (wcscmp(extensiontype[i], TextureName) == 0)
			{
				hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName, NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
				if (FAILED(hr))
				{
					Error::showDialog("WICTextureLoader�ł̓ǂݍ��݂Ɏ��s���܂���");
					return false;
				}
			}
		}
	}
	else
	{
		Error::showDialog("�e�N�X�`���t�@�C��������Ɋg���q���܂܂�Ă��܂���");
	}

	return true;
}

bool Texture::checkExtension( std::filesystem::path PathName)
{
	//�Ή����Ă���g���q�����邩�m�F
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//�g���q�����[�_�[�Ŏg�p�ł�����̂��m�F
		if (wcscmp(extensiontype[i], PathName.c_str())==0)
		{
			return true;
		}
	}

	return false;
}

void Texture::destroy()
{
	SAFE_RELEASE(textureresource_);
	SAFE_RELEASE(texture_);
}
