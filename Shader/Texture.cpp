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
	char tmp[MAX_PATH] = " ";

	//��������^�ϊ�
	wcstombs(tmp, TextureName, MAX_PATH);

	//�n���ꂽ�t�@�C�����Ɋg���q���܂܂�Ă��邩�m�F
	if (PathFileExists(tmp))
	{
		//�g���q���e�N�X�`�����[�_�[�ɑΉ����Ă��邩�m�F
		if (!checkExtension(TextureName))
		{
			Error::showDialog("�e�N�X�`�����[�_�[���Ή����Ă��Ȃ��g���q�ł�");
			return false;
		}
		if(PathFindExtension(tmp))
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName,&textureresource_, &texture_);
			if (FAILED(hr))
			{
				//DDS�œǂ߂Ȃ��ꍇWIC���g��
				hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName, NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
				if (FAILED(hr))
				{
					Error::showDialog("�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���");
					return false;
				}
				return false;
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

bool Texture::checkExtension(const wchar_t* PathName)
{
	//�Ή����Ă���g���q�����邩�m�F
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//�g���q�����[�_�[�Ŏg�p�ł�����̂��m�F
		if (wcscmp(extensionarray[i],PathName))
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
