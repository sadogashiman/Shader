#pragma once
class TextureFactory
{
private:
	TextureFactory();
	~TextureFactory();
	std::map<std::filesystem::path, ID3D11ShaderResourceView*>texmap_;
	std::map<std::filesystem::path, int>cntmap_;
	std::map<std::filesystem::path, ID3D11Resource*>resourcemap_;
public:
	TextureFactory(const TextureFactory&) = delete;
	TextureFactory& operator = (const TextureFactory&) = delete;
	TextureFactory(TextureFactory&&) = delete;
	TextureFactory& operator = (TextureFactory&&) = delete;

	//delete
	void deleteTexture(std::filesystem::path TextureName);
	void allDeleteTexture(std::filesystem::path TextureName);

	//get
	ID3D11ShaderResourceView* getTexture(std::filesystem::path TextureName);
	ID3D11Resource* getTextureParameter(std::filesystem::path TextureName);

	static inline  TextureFactory* getInstance()
	{
		static TextureFactory instance;
		return &instance;
	}



};

