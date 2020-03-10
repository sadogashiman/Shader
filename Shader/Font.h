#pragma once
#include"TextureFactory.h"
class Font
{
private:
	struct FontType
	{
		float left;
		float right;
		int size;
	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

	wchar_t filename[MAX_PATH];
	FontType* font_;
	float fontheight_;
	int spacesize_;

	bool loadFontData(const wchar_t* FontFileName);

public:

	bool init(const wchar_t* FontFileName, const wchar_t* TextureFileName, float FontHeight, int SpaceSize);
	void destroy();
	void buildVertexArray(void* Vertices, wchar_t* Sentence, float DrawX, float DrawY);

	//get
	inline ID3D11ShaderResourceView* getTexture() { return TextureFactory::getInstance()->getTexture(filename); }
	int getSentencePixelLength(wchar_t* Sentence);
};

