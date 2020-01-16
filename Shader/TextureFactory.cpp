#include "stdafx.h"
#include "TextureFactory.h"
#include"release.h"
#include"Texture.h"
#include"error.h"

TextureFactory::TextureFactory()
{
}

TextureFactory::~TextureFactory()
{
}

void TextureFactory::deleteTexture(const wchar_t* TextureName)
{
	if (--cntmap_[TextureName] <= 0)
	{
		auto itr = texmap_.find(TextureName);

		if (itr != texmap_.end())
		{
			SAFE_RELEASE(itr->second);
			texmap_.erase(TextureName);
			resourcemap_.erase(TextureName);
		}
	}
}

void TextureFactory::allDeleteTexture()
{
	auto end = texmap_.end();

	for (auto itr = texmap_.begin(); itr != end;)
	{
		SAFE_RELEASE(itr->second);
		itr = texmap_.erase(itr);
	}
	auto end2 = resourcemap_.end();

	for (auto itr = resourcemap_.begin(); itr != end2;)
	{
		SAFE_RELEASE(itr->second);
		itr = resourcemap_.erase(itr);
	}
}

ID3D11ShaderResourceView* TextureFactory::getTexture(const wchar_t* TextureName)
{
	auto itr = texmap_.find(TextureName);

	if (itr != texmap_.end())
	{
		cntmap_[TextureName]++;
		return itr->second;
	}
	else
	{
		ID3D11ShaderResourceView* texture;
		if (!Texture::getInstance()->init(TextureName))
		{
			Error::showDialog("テクスチャロードに失敗");
			return false;
		}
		if (!(texture = Texture::getInstance()->getTexture()))
		{
			return nullptr;
		}

		cntmap_[TextureName] = 1;
		texmap_[TextureName] = texture;
		resourcemap_[TextureName] = Texture::getInstance()->getTextureResource();
		return texture;
	}
}

ID3D11Resource* TextureFactory::getTextureParameter(const wchar_t* TextureName)
{
	auto itr = resourcemap_.find(TextureName);

	if (itr != resourcemap_.end())
	{
		cntmap_[TextureName]++;
		return itr->second;
	}
	else
	{
		ID3D11Resource* resource;
		Texture::getInstance()->init(TextureName);
		if (!(resource = Texture::getInstance()->getTextureResource()))
		{
			return nullptr;
		}

		cntmap_[TextureName] = 1;
		resourcemap_[TextureName] = resource;
		return resource;
	}
}
