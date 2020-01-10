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

	//文字列を型変換
	wcstombs(tmp, TextureName, MAX_PATH);

	//渡されたファイル名に拡張子が含まれているか確認
	if (PathFileExists(tmp))
	{
		//拡張子がテクスチャローダーに対応しているか確認
		if (!checkExtension(TextureName))
		{
			Error::showDialog("テクスチャローダーが対応していない拡張子です");
			return false;
		}

		//拡張子がdds又はDDSの場合DDSローダーを使用
		if (wcsstr(extensionarray[kDds],TextureName))
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName,&textureresource_, &texture_);
			if (FAILED(hr))
			{
				Error::showDialog("DDSTextureLoaderでの読み込みに失敗しました");
				return false;
			}
		}

		//WICTextureLoaderで読めるものはWICを使用
		for (int i = kPng; i < kEnd; i++)
		{
			if (wcsstr(extensionarray[i], TextureName))
			{
				hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName, NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
				if (FAILED(hr))
				{
					Error::showDialog("WICTextureLoaderでの読み込みに失敗しました");
					return false;
				}
			}
		}
	}
	else
	{
		Error::showDialog("テクスチャファイル文字列に拡張子が含まれていません");
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
		if (wcsstr(extensionarray[i],PathName))
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
