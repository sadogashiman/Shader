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

bool Texture::init(std::filesystem::path TextureName)
{
	HRESULT hr;

	//�n���ꂽ�t�@�C�����Ɋg���q���܂܂�Ă��邩�m�F
	if (TextureName.has_extension())
	{
		//�g���q���e�N�X�`�����[�_�[�ɑΉ����Ă��邩�m�F
		if (!checkExtension(TextureName.extension()))
		{
			Error::showDialog("�e�N�X�`�����[�_�[���Ή����Ă��Ȃ��g���q�ł�");
			return false;
		}

		//�g���q��dds����DDS�̏ꍇDDS���[�_�[���g�p
		if (extensionarray[kDds] == TextureName)
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName.c_str(), NULL, &texture_, NULL);
			if (FAILED(hr))
			{
				Error::showDialog("DDSTextureLoader�ł̓ǂݍ��݂Ɏ��s���܂���");
				return false;
			}
		}

		//WICTextureLoader�œǂ߂���̂�WIC���g�p
		for (int i = kPng; i < kEnd; i++)
		{
			if (extensionarray[i] == TextureName)
			{
				hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName.c_str(), NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
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
		return false;
	}

	return true;
}

bool Texture::checkExtension(std::filesystem::path PathName)
{
	//�Ή����Ă���g���q�����邩�m�F
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//�g���q�����[�_�[�Ŏg�p�ł�����̂��m�F
		if (extensionarray[i]==PathName)
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
