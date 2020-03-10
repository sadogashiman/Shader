#include "stdafx.h"
#include "Font.h"
#include"Support.h"

bool Font::loadFontData(const wchar_t* FontFileName)
{
	std::fstream fp;
	char tmp;

	//フォントバッファを作成
	font_ = new FontType[95];
	if (!font_)
	{
		Error::showDialog("フォントバッファのメモリ確保に失敗");
		return false;
	}

	//ファイルパスが有効か確認
	if (!Support::searchFile(FontFileName))
	{
		Error::showDialog("ファイルパスが無効です");
		return false;
	}

	//ファイル展開
	fp.open(FontFileName);
	if (fp.fail())
	{
		Error::showDialog("ファイル展開に失敗");
		return false;
	}

	for (int i = 0; i < 95; i++)
	{
		fp >> tmp;

		while (tmp != ' ')
		{
			fp >> tmp;
		}

		fp >> tmp;

		while (tmp != ' ')
		{
			fp >> tmp;
		}

		//データを配列に格納
		fp >> font_[i].left;
		fp >> font_[i].right;
		fp >> font_[i].size;
	}

	//ファイルを閉じる
	fp.close();

	return true;
}

bool Font::init(const wchar_t* FontFileName, const wchar_t* TextureFileName, float FontHeight, int SpaceSize)
{
	bool result;

	//メンバにフォントの高さを保存
	fontheight_ = FontHeight;

	//ピクセルサイズをメンバに保存
	spacesize_ = SpaceSize;

	//テクスチャファイル文字列をメンバにコピー
	wcscpy(filename, TextureFileName);

	return false;
}

void Font::destroy()
{
	//テクスチャ破棄
	TextureFactory::getInstance()->deleteTexture(filename);
}

void Font::buildVertexArray(void* Vertices, wchar_t* Sentence, float DrawX, float DrawY)
{
	VertexType* vertexptr;
	int numletters;
	int index;
	int letter;

	//ポインタをキャスト
	vertexptr = (VertexType*)Vertices;

	//長さを取得
	numletters = static_cast<int>(wcslen(Sentence));

	//インデックスを初期化
	index = 0;

	for (int i = 0; i < numletters; i++)
	{
		letter = static_cast<int>(Sentence[i]) - 32;

		if (letter == 0)
		{
			DrawX = DrawY + static_cast<float>(spacesize_);
		}
		else
		{
			vertexptr[index].position = Vector3(DrawX, DrawY, 0.0F);
			vertexptr[index].texture = Vector2(font_[letter].left, 0.0F);
			index++;

			vertexptr[index].position = Vector3(DrawX + font_[letter].size, DrawY - fontheight_, 0.0F);
			vertexptr[index].texture = Vector2(font_[letter].right, 0.0F);
			index++;

			vertexptr[index].position = Vector3(DrawX, DrawY - fontheight_, 0.0F);
			vertexptr[index].texture = Vector2(font_[letter].left, 1.0F);
			index++;

			vertexptr[index].position = Vector3(DrawX, DrawY, 0.0F);
			vertexptr[index].texture = Vector2(font_[letter].left, 0.0F);
			index++;

			vertexptr[index].position = Vector3(DrawX + font_[letter].size, DrawY, 0.0F);
			vertexptr[index].texture = Vector2(font_[letter].right, 0.0F);
			index++;

			vertexptr[index].position = Vector3(DrawX + font_[letter].size, DrawY - fontheight_, 0.0F);
			vertexptr[index].texture = Vector2(font_[letter].left, 1.0F);
			index++;

			//描画のX軸を更新
			DrawX = DrawX + font_[letter].size + 1.0F;
		}
	}
}

int Font::getSentencePixelLength(wchar_t* Sentence)
{

	int pixellength;
	int numletters;
	int letter;

	pixellength = 0;
	numletters = static_cast<int>(wcslen(Sentence));

	for (int i = 0; i < numletters; i++)
	{
		letter = (static_cast<int>(wcslen(Sentence)));

		if (letter == 0)
		{
			pixellength += spacesize_;
		}
		else
		{
			pixellength += (font_[letter].size + 1);
		}
	}

	return pixellength;
}
