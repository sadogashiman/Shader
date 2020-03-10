#include "stdafx.h"
#include "Font.h"
#include"Support.h"

bool Font::loadFontData(const wchar_t* FontFileName)
{
	std::fstream fp;
	char tmp;

	//�t�H���g�o�b�t�@���쐬
	font_ = new FontType[95];
	if (!font_)
	{
		Error::showDialog("�t�H���g�o�b�t�@�̃������m�ۂɎ��s");
		return false;
	}

	//�t�@�C���p�X���L�����m�F
	if (!Support::searchFile(FontFileName))
	{
		Error::showDialog("�t�@�C���p�X�������ł�");
		return false;
	}

	//�t�@�C���W�J
	fp.open(FontFileName);
	if (fp.fail())
	{
		Error::showDialog("�t�@�C���W�J�Ɏ��s");
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

		//�f�[�^��z��Ɋi�[
		fp >> font_[i].left;
		fp >> font_[i].right;
		fp >> font_[i].size;
	}

	//�t�@�C�������
	fp.close();

	return true;
}

bool Font::init(const wchar_t* FontFileName, const wchar_t* TextureFileName, float FontHeight, int SpaceSize)
{
	bool result;

	//�����o�Ƀt�H���g�̍�����ۑ�
	fontheight_ = FontHeight;

	//�s�N�Z���T�C�Y�������o�ɕۑ�
	spacesize_ = SpaceSize;

	//�e�N�X�`���t�@�C��������������o�ɃR�s�[
	wcscpy(filename, TextureFileName);

	return false;
}

void Font::destroy()
{
	//�e�N�X�`���j��
	TextureFactory::getInstance()->deleteTexture(filename);
}

void Font::buildVertexArray(void* Vertices, wchar_t* Sentence, float DrawX, float DrawY)
{
	VertexType* vertexptr;
	int numletters;
	int index;
	int letter;

	//�|�C���^���L���X�g
	vertexptr = (VertexType*)Vertices;

	//�������擾
	numletters = static_cast<int>(wcslen(Sentence));

	//�C���f�b�N�X��������
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

			//�`���X�����X�V
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
