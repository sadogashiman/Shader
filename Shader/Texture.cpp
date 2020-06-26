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
		Error::showDialog("Targaファイルへのファイルパスが無効です");
		return false;
	}

	//fopen関数のために型変換
	wcstombs(tmp, TargaFileName, MAX_PATH);

	//Targaファイル展開
	error = fopen_s(&fp, tmp, "rb");
	if (error != 0)
	{
		Error::showDialog("Targaファイルの展開に失敗");
		return false;
	}
	//ヘッダー部分を読み取り
	cnt = static_cast<unsigned int>(fread(&tgaheader, sizeof(TargaHeader), 1, fp));
	if (cnt != 1)
	{
		Error::showDialog("Targaファイルのヘッダー部分の読み取りに失敗");
		return false;
	}

	//ファイル情報を取得
	Height = static_cast<int>(tgaheader.height);
	Width = static_cast<int>(tgaheader.width);
	bpp = static_cast<int>(tgaheader.bpp);

	//32bitか確認
	if (bpp != 32)
	{
		Error::showDialog("ビット深度が32bitではありません");
		return false;
	}

	//データサイズを計算
	imagesize = Width * Height * 4;

	//配列を画像サイズに変更
	tgaimage.resize(imagesize);

	//画像データを読み込み
	cnt = static_cast<int>(fread(&tgaimage[0], 1, imagesize, fp));
	if (cnt != imagesize)
	{
		Error::showDialog("画像データの読み込みが正常に終了しませんでした");
		return false;
	}

	//ファイルクローズ
	error = fclose(fp);
	if (error != 0)
	{
		Error::showDialog("ファイルクローズに失敗");
		return false;
	}

	//メンバのデータ保持用配列のサイズを変更
	targadata_.resize(imagesize);

	//インデックスを初期化
	index = 0;

	//カラー情報用インデックス変数
	int colorindex;

	//カラーインデックスを計算
	colorindex = (Width * Height * 4) - (Width * 4);

	//画像データを保存
	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			targadata_[index] = tgaimage[colorindex + 2];		//r
			targadata_[index + 1] = tgaimage[colorindex + 1];	//g
			targadata_[index + 2] = tgaimage[colorindex];		//b
			targadata_[index + 3] = tgaimage[colorindex + 3];	//a

			//インデックスを加算
			colorindex += 4;
			index += 4;
		}

		//先頭に戻す
		colorindex -= (Width*8);
	}

	//配列解放
	tgaimage.clear();

	return true;
}

bool Texture::createTargaTexture(const int Width, const int Height)
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC texturedesc;
	unsigned int rowpitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;

	//テクスチャの設定
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

	//空のテクスチャを作成
	hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&texturedesc, NULL, texture2d_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("空のテクスチャ情報の作成に失敗");
		return false;
	}

	//ピッチを設定
	rowpitch = (Width * 4) * sizeof(unsigned char);

	//画像データをテクスチャにコピー
	Direct3D::getInstance()->getContext()->UpdateSubresource(texture2d_.Get(), 0, NULL,&targadata_[0], rowpitch, 0);

	//シェーダーリソースの設定
	srvdesc.Format = texturedesc.Format;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Texture2D.MostDetailedMip = 0;
	srvdesc.Texture2D.MipLevels = -1;

	//シェーダーリソースを作成
	hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(texture2d_.Get(), &srvdesc, &texture_);
	if (FAILED(hr))
	{
		Error::showDialog("シェーダーリソースの作成に失敗");
		return false;
	}

	//mipmapを作成
	Direct3D::getInstance()->getContext()->GenerateMips(texture_);

	//不要になったのでデータ解放
	targadata_.clear();

	return true;
}

bool Texture::init(const wchar_t* TextureName)
{
	HRESULT hr;
	int width, height;
	char tmp[MAX_PATH] = " ";

	//文字列を型変換
	wcstombs(tmp, TextureName, MAX_PATH);

	//ファイルパスが有効か確認
	if (Support::searchFile(TextureName))
	{
		//拡張子がテクスチャローダーに対応しているか確認
		if (!checkExtension(TextureName))
		{
			Error::showDialog("テクスチャローダーが対応していない拡張子です");
			return false;
		}

		//ファイル拡張子から適切なローダーを判別して使用
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
					Error::showDialog("Targa画像データの作成");
					return false;
				}
			}
			else
			{
				Error::showDialog("Trgaファイルのロードに失敗");
				return false;
			}
		}
		else if (wcscmp(wcsrchr(TextureName, L'.'), extensionarray[kDds]) == 0)
		{
			hr = CreateDDSTextureFromFile(Direct3D::getInstance()->getDevice(), TextureName, &textureresource_, &texture_);
			if (FAILED(hr))
			{
				Error::showDialog("DDSテクスチャの作成に失敗しました");
				return false;
			}

		}
		else if (wcscmp(wcsrchr(TextureName, L'.'), extensionarray[kDds]) == 0)
		{
			hr = CreateWICTextureFromFileEx(Direct3D::getInstance()->getDevice(), TextureName, NULL, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, WIC_LOADER_DEFAULT, &textureresource_, &texture_);
			if (FAILED(hr))
			{
				Error::showDialog("WICテクスチャの作成に失敗しました");
				return false;
			}
		}
	}
	else
	{
		Error::showDialog("ファイルパスが無効です");
		return false;
	}

	return true;
}

bool Texture::checkExtension(const wchar_t* PathName)
{
	//対応している拡張子があるか確認
	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		//拡張子がローダーで使用できるものか確認
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
