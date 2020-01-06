#pragma once
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

	bool init(std::filesystem::path TextureName);
	void destroy();

	inline ID3D11ShaderResourceView* getTexture()const { return texture_; }        //テクスチャを返す
	inline ID3D11Resource* getTextureResource()const { return textureresource_; }  //テクスチャリソースを返す
	static Texture* getInstance()
	{
		static Texture instance;
		return &instance;
	}
};

