#pragma once
enum ExtensionType
{
	kDds,
	kPng,
	kTiff,
	kGif,
	kTga,
	kEnd
};
class Texture
{
private:
	//TextureLoaderが対応している拡張子
	const wchar_t* extensionarray[kExtensionTypeNum] =
	{
		L".dds",
		L".png",
		L".tiff",
		L".gif",
		L".tga"
	};

	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	ID3D11ShaderResourceView* texture_;
	ID3D11Resource* textureresource_;
	ComPtr<ID3D11Texture2D>texture2d_;

	std::vector<unsigned char> targadata_;

	Texture();
	~Texture();

	bool loadTargaFile(const wchar_t* TargaFileName,int& Width,int& Height);
	bool createTargaTexture(const int Width,const int Height);
	bool checkExtension(const wchar_t* PathName);

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

