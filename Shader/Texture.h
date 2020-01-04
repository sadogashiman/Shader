#pragma once
const int kExtensionTypeNum = 4;
//WICTextureLoaderが対応している拡張子
const wchar_t* extensiontype[] =
{
	L".dds",
	L".png",
	L".tiff",
	L".gif"
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
	Texture();
	~Texture();
public:
	Texture(const Texture&) = delete;
	Texture& operator = (const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator = (Texture&&) = delete;

	bool init(const wchar_t* TextureName);
	void destroy();
	inline ID3D11ShaderResourceView* getTexture()const { return texture_; }        //テクスチャを返す
	inline ID3D11Resource* getTextureResource()const { return textureresource_; }  //テクスチャリソースを返す
	static Texture* getInstance()
	{
		static Texture instance;
		return &instance;
	}
};

