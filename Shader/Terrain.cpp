#include "stdafx.h"
#include "Terrain.h"
#include"Support.h"
#include"Direct3D.h"


Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

bool Terrain::init(const wchar_t* ModelFileName)
{
	bool result;

	//テレイン情報の取得
	result = loadTerrainDataFile(L"Resource/setup.txt");
	if (!result)
	{
		Error::showDialog("テレイン情報の取得に失敗");
		return false;
	}

	//ビットマップの読み込み
	result = loadBitmapHeightMap();
	if (!result)
	{
		Error::showDialog("ビットマップファイルの読み込みに失敗");
		return false;
	}

	//読み込んだ設定をテレインに反映
	setTerrainCoordinate();

	//地形情報を生成
	result = buildTerrainModel();
	if (!result)
	{
		Error::showDialog("地形情報の生成に失敗");
		return false;
	}

	//地形情報を生成したのでheightmapを削除
	destroyHightMap();

	//バッファの初期化
	result = initbuffer();
	if (!result)
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}

	//バッファが作成されたのでモデル情報を破棄
	destroyTerrainModel();

	return true;
}

void Terrain::render()
{
	unsigned int stride;
	unsigned int offset;

	//幅とオフセットを設定
	stride = sizeof(VertexType);
	offset = 0;

	//頂点バッファをセット
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//インデックスバッファをセット
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//プリミティブのモードを設定
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Terrain::destroy()
{
	destroyHightMap();
	destroyTerrainModel();
}

bool Terrain::initbuffer()
{
	HRESULT hr;
	std::vector<VertexType>vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	int terrainwidth;
	int terrainheight;
	Vector4 color;
	Vector3 position;

	//テレイングリッドのサイズを設定
	terrainwidth = 256;
	terrainheight = 256;

	//グリッドの色を設定
	color = Vector4::One;

	//頂点数を計算
	vertexcnt_ = (terrainwidth - 1) * (terrainheight - 1) * 6;

	//インデックス数を設定
	indexcnt_ = vertexcnt_;

	//頂点配列を作成
	vertices.resize(vertexcnt_);

	//インデックス配列を作成
	indices.resize(indexcnt_);

	//頂点配列にデータをロード
	for (int i = 0; i < vertexcnt_; i++)
	{
		vertices[i].position = Vector3(model_[i].x, model_[i].y, model_[i].z);
		vertices[i].color = color;
		indices[i] = i;
	}

	//頂点バッファを設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//サブリソースへのポインタを作成
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点バッファの作成に失敗");
		return false;
	}

	//インデックスバッファを設定
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcnt_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//サブリソースへのポインタを作成
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("インデックスバッファの作成に失敗");
		return false;
	}

	return true;
}

bool Terrain::loadTerrainDataFile(const wchar_t* FileName)
{
	std::ifstream fin;
	char input;

	//データファイル展開
	if (!Support::searchFile(FileName))
	{
		Error::showDialog("データファイルへのファイルパスが無効です");
		return false;
	}

	fin.open(FileName);
	if (fin.fail())
	{
		Error::showDialog("データファイルの展開に失敗");
		return false;
	}

	//テレインファイル名まで飛ばす
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//ファイル名を読み取り
	fin >> terrainfilename_;

	//高さ情報まで飛ばす
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//高さを読み取る
	fin >> terrainheight_;

	//幅情報まで飛ばす
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//幅情報を読み取り
	fin >> terrainwidth_;

	//スケール値まで飛ばす
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//スケールを取得
	fin >> heightscale_;

	//ファイルを閉じる
	fin.close();

	return true;
}

bool Terrain::loadBitmapHeightMap()
{
	int imagesize;
	int index;
	int error;
	FILE* fp;
	unsigned long long cnt;
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	std::vector<unsigned char> bitmapimage;
	unsigned char height;

	//情報格納配列を作成
	heightmap_.resize(terrainheight_ * terrainwidth_);

	//ビットマップファイルをバイナリで展開
	if (!Support::searchFile(terrainfilename_))
	{
		Error::showDialog("ビットマップファイルパスが無効です");
		return false;
	}

	//ビットマップ展開
	error = fopen_s(&fp, terrainfilename_, "rb");
	if (error != 0)
	{
		Error::showDialog("ファイル展開に失敗");
		return false;
	}

	//ビットマップのヘッダー部分を読み取り
	cnt = fread(&bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	if (cnt != 1)
	{
		Error::showDialog("ヘッダー部分の読み取りに失敗");
		return false;
	}

	//ビットマップ情報を読み取り
	cnt = fread(&bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);
	if (cnt != 1)
	{
		Error::showDialog("ビットマップ情報ヘッダーの読み取りに失敗");
		return false;
	}

	//heightマップの次元がテレインと同じか確認
	if ((bitmapinfoheader.biHeight != terrainheight_) || (bitmapinfoheader.biWidth != terrainwidth_))
	{
		Error::showDialog("Heightマップの次元とプログラム側の地形の次元が一致しません");
		return false;
	}

	//画像データサイズを取得
	imagesize = terrainheight_ * ((terrainwidth_ * 3) + 1);

	//画像データにメモリを割り当て
	bitmapimage.resize(imagesize);

	//ファイルポインタを先頭へ移動
	fseek(fp, bitmapfileheader.bfOffBits, SEEK_SET);

	//画像データを読み取り
	cnt = fread(&bitmapimage[0], 1, imagesize, fp);
	if (cnt != imagesize)
	{
		Error::showDialog("画像データの読み込みに失敗");
		return false;
	}

	//ファイルを閉じる
	error = fclose(fp);
	if (error != 0)
	{
		Error::showDialog("ファイルクローズに失敗");
		return false;
	}

	//バッファの位置決め
	int k = 0;

	for (int i = 0; i < terrainheight_; i++)
	{
		for (int j = 0; j < terrainwidth_; j++)
		{
			//ビットマップは上下が逆なので下から上へロード
			index = (terrainwidth_ * (terrainheight_ - 1 - i)) + j;

			//画像からグレースケールのピクセル値を取得
			height = bitmapimage[k];

			//高さ情報としてピクセル値を格納
			heightmap_[index].y = static_cast<float>(height);

			//ビットマップデータインデックスをインクリメント
			k += 3;
		}

		//ビットマップを補正
		k++;
	}

	//ビットマップ画像データを解放
	bitmapimage.clear();

	return true;
}

void Terrain::setTerrainCoordinate()
{
	int index;

	//座標を調整
	for (int i = 0; i < terrainheight_; i++)
	{
		for (int j = 0; j < terrainwidth_; j++)
		{
			index = (terrainwidth_ * i) + j;

			//X座標・Z座標を設定
			heightmap_[index].x = static_cast<float>(j);
			heightmap_[index].z = static_cast<float>(i);

			//震度情報を正の値だけになるよう移動
			heightmap_[index].z += static_cast<float>(terrainheight_ - 1);

			//高さをスケーリング
			heightmap_[index].y /= heightscale_;
		}
	}

}

bool Terrain::buildTerrainModel()
{
	int index;
	int index1;
	int index2;
	int index3;
	int index4;
	
	//頂点数を計算
	vertexcnt_ = (terrainheight_ - 1) * (terrainwidth_ - 1) * 6;

	//頂点配列を作成
	model_.resize(vertexcnt_);
	
	index = 0;

	int width = terrainwidth_ - 1;
	int height = terrainheight_ - 1;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			index1 = (terrainwidth_ * i) + j;				//左上
			index2 = (terrainwidth_ * i) + (j + 1);			//右上
			index3 = (terrainwidth_ * (i + 1)) + j;			//左下
			index4 = (terrainwidth_ * (i + 1)) + (j + 1);	//右下

			//三角形を生成
			model_[index].x = heightmap_[index1].x;
			model_[index].y = heightmap_[index1].y;
			model_[index].z = heightmap_[index1].z;
			index++;

			model_[index].x = heightmap_[index2].x;
			model_[index].y = heightmap_[index2].y;
			model_[index].z = heightmap_[index2].z;
			index++;

			model_[index].x = heightmap_[index3].x;
			model_[index].y = heightmap_[index3].y;
			model_[index].z = heightmap_[index3].z;
			index++;

			model_[index].x = heightmap_[index3].x;
			model_[index].y = heightmap_[index3].y;
			model_[index].z = heightmap_[index3].z;
			index++;

			model_[index].x = heightmap_[index2].x;
			model_[index].y = heightmap_[index2].y;
			model_[index].z = heightmap_[index2].z;
			index++;

			model_[index].x = heightmap_[index4].x;
			model_[index].y = heightmap_[index4].y;
			model_[index].z = heightmap_[index4].z;
			index++;
		}
	}

	return true;
}

void Terrain::destroyHightMap()
{
	//hrightマップを解放
	if (heightmap_.size() > 0)
	{
		heightmap_.clear();
	}
}

void Terrain::destroyTerrainModel()
{
	//テレインモデル解放
	if (model_.size() > 0)
	{
		model_.clear();
	}
}
