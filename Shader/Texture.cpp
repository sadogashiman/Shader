#include "stdafx.h"
#include "Texture.h"
#include"release.h"
#include"Direct3D.h"
#include"error.h"



Texture::Texture()
{
	ZeroMemory(this, sizeof(Texture));
}

Texture::~Texture()
{
}

bool Texture::init(const wchar_t* TextureName)
{
	HRESULT hr;
	char tmp[MAX_PATH] = " ";

	//文字列を型変換
	wcstombs(tmp, TextureName, MAX_PATH);

	//ファイルパスが有効か確認
	if (PathFindExtension(tmp))
	{
		//拡張子がテクスチャローダーに対応しているか確認
		if (!checkExtension(TextureName))
		{
			Error::showDialog("テクスチャローダーが対応していない拡張子です");
			return false;
		}
		if(PathFileExists(tmp))
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName,&textureresource_, &texture_);
			if (FAILED(hr))
			{
				//DDSで読めない場合WICを使う
				hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName, NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
				if (FAILED(hr))
				{
					Error::showDialog("テクスチャの読み込みに失敗しました");
					return false;
				}
				return false;
			}
		}
	}
	else
	{
		Error::showDialog("ファイル名に拡張子がありません");
		return false;
	}

	return true;
}

bool Texture::checkExtension(const wchar_t* PathName)
{
	//対応している拡張子があるか確認
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//拡張子がローダーで使用できるものか確認
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
