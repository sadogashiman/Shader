#include "stdafx.h"
#include "TextureFactory.h"
#include"release.h"
#include"Texture.h"

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

void TextureFactory::allDeleteTexture(const wchar_t* TextureName)
{

	auto end = texmap_.end();

	for (auto itr = texmap_.begin(); itr != end;)
	{
		SAFE_RELEASE(itr->second);
		itr = texmap_.erase(itr);
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
		//テクスチャがマップ内に存在する場合
		ID3D11ShaderResourceView* texture;
		Texture::getInstance()->init(TextureName);
		if (!(texture = Texture::getInstance()->getTexture()))
		{
			return nullptr;
		}

		cntmap_[TextureName] = 1;
		texmap_[TextureName] = texture;
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
		//テクスチャがマップ内に存在する場合
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
