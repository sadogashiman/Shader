#include "stdafx.h"
#include "Texture.h"
#include"Direct3D.h"
#include"Support.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

bool Texture::loadTargaFile(const wchar_t* TargaFileName, int& Width, int& Height)
{
	int error;
	int bpp;
	int imagesize;
	int index;
	unsigned int cnt;
	FILE* fp;
	TargaHeader tgaheader;
	std::vector<unsigned char> tgaimage;
	char tmp[MAX_PATH];

	if (!Support::searchFile(TargaFileName))
	{
		Error::showDialog("Targa�t�@�C���ւ̃t�@�C���p�X�������ł�");
		return false;
	}

	//fopen�֐��̂��߂Ɍ^�ϊ�
	wcstombs(tmp, TargaFileName, MAX_PATH);

	//Targa�t�@�C���W�J
	error = fopen_s(&fp, tmp, "rb");
	if (error != 0)
	{
		Error::showDialog("Targa�t�@�C���̓W�J�Ɏ��s");
		return false;
	}
	//�w�b�_�[������ǂݎ��
	cnt = static_cast<unsigned int>(fread(&tgaheader, sizeof(TargaHeader), 1, fp));
	if (cnt != 1)
	{
		Error::showDialog("Targa�t�@�C���̃w�b�_�[�����̓ǂݎ��Ɏ��s");
		return false;
	}

	//�t�@�C�������擾
	Height = static_cast<int>(tgaheader.height);
	Width = static_cast<int>(tgaheader.width);
	bpp = static_cast<int>(tgaheader.bpp);

	//32bit���m�F
	if (bpp != 32)
	{
		Error::showDialog("�r�b�g�[�x��32bit�ł͂���܂���");
		return false;
	}

	//�f�[�^�T�C�Y���v�Z
	imagesize = Width * Height * 4;

	//�z����摜�T�C�Y�ɕύX
	tgaimage.resize(imagesize);

	//�摜�f�[�^��ǂݍ���
	cnt = static_cast<int>(fread(&tgaimage[0], 1, imagesize, fp));
	if (cnt != imagesize)
	{
		Error::showDialog("�摜�f�[�^�̓ǂݍ��݂�����ɏI�����܂���ł���");
		return false;
	}

	//�t�@�C���N���[�Y
	error = fclose(fp);
	if (error != 0)
	{
		Error::showDialog("�t�@�C���N���[�Y�Ɏ��s");
		return false;
	}

	//�����o�̃f�[�^�ێ��p�z��̃T�C�Y��ύX
	targadata_.resize(imagesize);

	//�C���f�b�N�X��������
	index = 0;

	//�J���[���p�C���f�b�N�X�ϐ�
	int colorindex;

	//�J���[�C���f�b�N�X���v�Z
	colorindex = (Width * Height * 4) - (Width * 4);

	//�摜�f�[�^��ۑ�
	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			targadata_[index] = tgaimage[colorindex + 2];		//r
			targadata_[index + 1] = tgaimage[colorindex + 1];	//g
			targadata_[index + 2] = tgaimage[colorindex];		//b
			targadata_[index + 3] = tgaimage[colorindex + 3];	//a

			//�C���f�b�N�X�����Z
			colorindex += 4;
			index += 4;
		}

		//�擪�ɖ߂�
		colorindex -= (Width*8);
	}

	//�z����
	tgaimage.clear();

	return true;
}

bool Texture::createTargaTexture(const int Width, const int Height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC texturedesc;
	unsigned int rowpitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;

	//�e�N�X�`���̐ݒ�
	texturedesc.Height = Height;
	texturedesc.Width = Width;
	texturedesc.MipLevels = 0;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.SampleDesc.Quality = 0;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texturedesc.CPUAccessFlags = 0;
	texturedesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//��̃e�N�X�`�����쐬
	hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&texturedesc, NULL, texture2d_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("��̃e�N�X�`�����̍쐬�Ɏ��s");
		return false;
	}

	//�s�b�`��ݒ�
	rowpitch = (Width * 4) * sizeof(unsigned char);

	//�摜�f�[�^���e�N�X�`���ɃR�s�[
	Direct3D::getInstance()->getContext()->UpdateSubresource(texture2d_.Get(), 0, NULL,&targadata_[0], rowpitch, 0);

	//�V�F�[�_�[���\�[�X�̐ݒ�
	srvdesc.Format = texturedesc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MostDetailedMip = 0;
	srvdesc.Texture2D.MipLevels = -1;

	//�V�F�[�_�[���\�[�X���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(texture2d_.Get(), &srvdesc, &texture_);
	if (FAILED(hr))
	{
		Error::showDialog("�V�F�[�_�[���\�[�X�̍쐬�Ɏ��s");
		return false;
	}

	//mipmap���쐬
	Direct3D::getInstance()->getContext()->GenerateMips(texture_);

	//�s�v�ɂȂ����̂Ńf�[�^���
	targadata_.clear();

	return true;
}

bool Texture::init(const wchar_t* TextureName)
{
	HRESULT hr;
	int width, height;
	char tmp[MAX_PATH] = " ";

	//��������^�ϊ�
	wcstombs(tmp, TextureName, MAX_PATH);

	//�t�@�C���p�X���L�����m�F
	if (Support::searchFile(TextureName))
	{
		//�g���q���e�N�X�`�����[�_�[�ɑΉ����Ă��邩�m�F
		if (!checkExtension(TextureName))
		{
			Error::showDialog("�e�N�X�`�����[�_�[���Ή����Ă��Ȃ��g���q�ł�");
			return false;
		}

		//�t�@�C���g���q����K�؂ȃ��[�_�[�𔻕ʂ��Ďg�p
		if (wcscmp(wcsrchr(TextureName, L'.'), extensionarray[kTga])==0)
		{
			if (loadTargaFile(TextureName, width, height))
			{
				if (createTargaTexture(width, height))
				{
					return true;
				}
				else
				{
					Error::showDialog("Targa�摜�f�[�^�̍쐬");
					return false;
				}
			}
			else
			{
				Error::showDialog("Trga�t�@�C���̃��[�h�Ɏ��s");
				return false;
			}
		}
		else if (wcscmp(wcsrchr(TextureName, L'.'), extensionarray[kDds]) == 0)
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName, &textureresource_, &texture_);
			if (FAILED(hr))
			{
				Error::showDialog("DDS�e�N�X�`���̍쐬�Ɏ��s���܂���");
				return false;
			}

		}
		else if (wcscmp(wcsrchr(TextureName, L'.'), extensionarray[kDds]) == 0)
		{
			hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName, NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
			if (FAILED(hr))
			{
				Error::showDialog("WIC�e�N�X�`���̍쐬�Ɏ��s���܂���");
				return false;
			}
		}
	}
	else
	{
		Error::showDialog("�t�@�C���p�X�������ł�");
		return false;
	}

	return true;
}

bool Texture::checkExtension(const wchar_t* PathName)
{
	//�Ή����Ă���g���q�����邩�m�F
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//�g���q�����[�_�[�Ŏg�p�ł�����̂��m�F
		if (wcscmp(extensionarray[i], PathName))
		{
			return true;
		}
	}

	return false;
}

void Texture::destroy()
{
	SAFE_RELEASE(textureresource_);
	SAFE_RELEASE(texture_);
}
