#include "stdafx.h"
#include "TextureFactory.h"
#include"Direct3D.h"

TextureFactory::TextureFactory()
{
}

TextureFactory::~TextureFactory()
{
}

bool TextureFactory::loadDDSTexture(std::string FileName)
{

	return false;
}

void TextureFactory::deleteTexture(std::string TextureName)
{
}

void TextureFactory::allDeleteTexture(std::string TextureName)
{
}

ID3D11ShaderResourceView* TextureFactory::getTexture(std::string TextureName)
{
	auto itr = texmap_.find(TextureName);

	if (itr != texmap_.end())
	{
		cntmap_[TextureName]++;
		return itr->second;
	}
	else
	{


	}

	return nullptr;
}

ID3D11Resource* TextureFactory::getTextureParameter(std::string TextureName)
{
	return nullptr;
}
