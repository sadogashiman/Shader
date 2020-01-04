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

	//どのローダーを使用するか拡張子で判断
	std::filesystem::path texturepath = TextureName;

	//渡されたファイル名に拡張子が含まれているか確認
	if (texturepath.has_extension())
	{
		//拡張子がテクスチャローダーに対応しているか確認
		if (!checkExtension(texturepath.extension()))
		{
			Error::showDialog("テクスチャローダーが対応していない拡張子です");
			return false;
		}
		
		//拡張子がdds又はDDSの場合DDSローダーを使用
		if(wcscmp(extensiontype[kDds],TextureName)==0)
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName, NULL, &texture_, NULL);
			if (FAILED(hr))
			{
				Error::showDialog("DDSTextureLoaderでの読み込みに失敗しました");
				return false;
			}
		}

		//WICTextureLoaderで読めるものはWICを使用
		for (int i = kPng; i = kGif; i++)
		{
			if (wcscmp(extensiontype[i], TextureName) == 0)
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
	}

	return true;
}

bool Texture::checkExtension( std::filesystem::path PathName)
{
	//対応している拡張子があるか確認
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//拡張子がローダーで使用できるものか確認
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
