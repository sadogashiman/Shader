#pragma once
const int texturenum = 3;
class TextureArray
{
private:
	ID3D11ShaderResourceView* texture_[texturenum];
	int texcnt_;
public:
	TextureArray();
	~TextureArray();
	bool init(const wchar_t* Texture1, const wchar_t* Texture2, const wchar_t* Alpha = nullptr);
	void destroy();
	inline ID3D11ShaderResourceView** const getTextureArray() { return texture_; }


};

