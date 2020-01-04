#pragma once
class TextureFactory
{
private:
	TextureFactory();
	~TextureFactory();
	std::map<const wchar_t*, ID3D11ShaderResourceView*>texmap_;
	std::map<const wchar_t*, int>cntmap_;
	std::map<const wchar_t*, ID3D11Resource*>resourcemap_;
public:
	TextureFactory(const TextureFactory&) = delete;
	TextureFactory& operator = (const TextureFactory&) = delete;
	TextureFactory(TextureFactory&&) = delete;
	TextureFactory& operator = (TextureFactory&&) = delete;

	//delete
	void deleteTexture(const wchar_t* TextureName);
	void allDeleteTexture(const wchar_t* TextureName);

	//get
	ID3D11ShaderResourceView* getTexture(const wchar_t* TextureName);
	ID3D11Resource* getTextureParameter(const wchar_t* TextureName);

	static inline  TextureFactory* getInstance()
	{
		static TextureFactory instance;
		return &instance;
	}



};

