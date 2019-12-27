#pragma once
const int kExtensionTypeNum = 4;
//WICTextureLoader‚ª‘Î‰ž‚µ‚Ä‚¢‚éŠg’£Žq
const std::string extensiontype[] =
{
	".dds",
	".png",
	".tiff",
	".gif"
};

enum ExtensionType
{
	kDds,
	kPng,
	kTiff,
	kGif,
};

class Texture
{
private:
	ID3D11ShaderResourceView* texture_;
	ID3D11Resource* textureresource_;
	bool checkExtension(std::filesystem::path PathName);
public:
	Texture();
	~Texture();
	bool init(std::string TextureName);
	void destroy();
	inline ID3D11ShaderResourceView* getTexture()const { return texture_; }

};

