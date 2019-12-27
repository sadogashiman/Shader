#pragma once
class TextureFactory
{
private:
	TextureFactory();
	~TextureFactory();
	std::map<std::string, ID3D11ShaderResourceView*>texmap_;
	std::map<std::string, int>cntmap_;
	std::map<std::string, ID3D11Resource*>resourcemap_;
	bool loadDDSTexture(std::string FileName);
public:
	TextureFactory(const TextureFactory&) = delete;
	TextureFactory& operator = (const TextureFactory&) = delete;
	TextureFactory(TextureFactory&&) = delete;
	TextureFactory& operator = (TextureFactory&&) = delete;

	//delete
	void deleteTexture(std::string TextureName);
	void allDeleteTexture(std::string TextureName);

	//get
	ID3D11ShaderResourceView* getTexture(std::string TextureName);
	ID3D11Resource* getTextureParameter(std::string TextureName);

	static inline  TextureFactory* getInstance()
	{
		static TextureFactory instance;
		return &instance;
	}



};

