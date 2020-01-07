#pragma once
enum ExtensionType
{
	kDds,
	kPng,
	kTiff,
	kGif,
	kEnd
};
class Texture
{
private:
	ID3D11ShaderResourceView* texture_;
	ID3D11Resource* textureresource_;
	bool checkExtension(const wchar_t* PathName);
	Texture();
	~Texture();
	//WICTextureLoader���Ή����Ă���g���q
	const wchar_t* extensionarray[kExtensionTypeNum] =
	{
		L".dds",
		L".png",
		L".tiff",
		L".gif"
	};
public:
	Texture(const Texture&) = delete;
	Texture& operator = (const Texture&) = delete;
	Texture(Texture&&) = delete;
	Texture& operator = (Texture&&) = delete;

	bool init(const wchar_t* TextureName);
	void destroy();

	inline ID3D11ShaderResourceView* getTexture()const { return texture_; }        //�e�N�X�`����Ԃ�
	inline ID3D11Resource* getTextureResource()const { return textureresource_; }  //�e�N�X�`�����\�[�X��Ԃ�
	static Texture* getInstance()
	{
		static Texture instance;
		return &instance;
	}
};

