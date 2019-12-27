#include "stdafx.h"
#include "TextureArray.h"
#include"release.h"
#include"Direct3D.h"
TextureArray::TextureArray()
{
	texcnt_ = 0;
}

TextureArray::~TextureArray()
{
}

bool TextureArray::init( const wchar_t* Texture1, const wchar_t* Texture2, const wchar_t* Alpha)
{
	HRESULT hr;
	size_t maxsize = 0;
	DDS_ALPHA_MODE* alphamode = nullptr;

	hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), Texture1, NULL, &texture_[0], NULL);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		texcnt_++;
	}

	hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), Texture2, NULL, &texture_[1], NULL);
	if (FAILED(hr))
	{
		return false;
	}
	else
	{
		texcnt_++;
	}

	if (Alpha)
	{
		hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), Alpha, NULL, &texture_[2], NULL);
		if (FAILED(hr))
		{
			return false;
		}
	}


	return true;
}

void TextureArray::destroy()
{
	for (int i = 0; i < texcnt_; i++)
	{
		SAFE_RELEASE(texture_[i]);
	}
}
