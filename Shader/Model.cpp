#include "stdafx.h"
#include "model.h"
#include"Direct3D.h"
#include"Support.h"
#include"Texture.h"



Model::Model()
{
	ZeroMemory(&texturefilename_, sizeof(texturefilename_));
	ZeroMemory(&mapfilename_, sizeof(mapfilename_));
}

Model::~Model()
{
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//ファイル名を保存
	wcscpy(texturefilename_[0], TextureFileName);

	if (!initBuffer())
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}

	return true;
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//ファイル名を保存
	wcscpy(texturefilename_[0], TextureFileName1);
	wcscpy(texturefilename_[1], TextureFileName2);

	if (!initBuffer())
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}
	return false;
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2, const wchar_t* MaskFileName)
{
	//モデルデータ読み込み
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//ファイル名を保存
	wcscpy(texturefilename_[0], TextureFileName1);
	wcscpy(texturefilename_[1], TextureFileName2);
	wcscpy(mapfilename_, MaskFileName);

	if (!initBuffer())
	{
		Error::showDialog("バッファの初期化に失敗");
		return false;
	}
	return false;
}

void Model::destroy()
{
	//このモデルが使用したテクスチャを解放させる
	for (int i = 0; i < kMaxTexture; i++)
	{
		if (texturefilename_[i] != NULL)
			TextureFactory::getInstance()->deleteTexture(texturefilename_[0]);
	}

	if (mapfilename_ != NULL)
	{
		TextureFactory::getInstance()->deleteTexture(mapfilename_);
	}

}

void Model::render()
{
	//グラフィックスパイプラインに頂点バッファをインデックスバッファを配置
	renderBuffer();
}

bool Model::initBuffer()
{
	std::vector<Vertextype> vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbuffdesc;
	D3D11_BUFFER_DESC indexbuffdesc;
	D3D11_SUBRESOURCE_DATA vertexdata, indexdata;
	HRESULT hr;

	//頂点配列を作成
	vertices.resize(vertexcount_);

	//インデックス配列の作成
	indices.resize(indexcount_);

	//頂点配列にデータをロード
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = Vector3(model_[i].x, model_[i].y, model_[i].z);
		vertices[i].texture = Vector2(model_[i].tu, model_[i].tv);
		vertices[i].normal = Vector3(model_[i].nx, model_[i].ny, model_[i].nz);

		indices[i] = i;
	}

	//静的頂点バッファの設定
	vertexbuffdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffdesc.ByteWidth = sizeof(Vertextype) * vertexcount_;
	vertexbuffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbuffdesc.CPUAccessFlags = 0;
	vertexbuffdesc.MiscFlags = 0;
	vertexbuffdesc.StructureByteStride = 0;

	//サブリソースに頂点データへのポインターを与える
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, vertexbuff_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//静的インデックスバッファの設定
	indexbuffdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbuffdesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexbuffdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbuffdesc.CPUAccessFlags = 0;
	indexbuffdesc.MiscFlags = 0;
	indexbuffdesc.StructureByteStride = 0;

	//サブリソースにインデックスデータへのポインタを与える
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbuffdesc, &indexdata, indexbuff_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Model::renderBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//頂点バッファのストライドとオフセットを設定
	stride = sizeof(Vertextype);
	offset = 0;

	//入力アセンブラで頂点バッファをアクティブにしてレンダリング可能に
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuff_.GetAddressOf(), &stride, &offset);

	//入力アセンブラでインデックスバッファをアクティブにしてレンダリング可能に
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuff_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//頂点バッファからレンダリングされるプリミティブのタイプ
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::loadModel(const wchar_t* ModelFileName)
{
	std::fstream fp;
	char input;

	//ファイルパスが有効か確認
	if (!Support::searchFile(ModelFileName))
	{
		return false;
	}

	//ファイルパスが有効なのを確認したので展開
	fp.open(ModelFileName, std::ios::beg | std::ios::in);

	if (fp.fail())
	{
		return false;
	}

	//頂点の数を取得
	fp.get(input);
	while (input != ':')
	{
		fp.get(input);
	}

	//頂点カウントを読み取り
	fp >> vertexcount_;

	//インデックスの数を頂点の数と同じに設定
	indexcount_ = vertexcount_;

	//読み込まれた頂点数を使用してモデル配列を作成
	model_.resize(vertexcount_);

	//データを先頭まで読み取る
	fp.get(input);
	while (input != ':')
	{
		fp.get(input);
	}

	//改行を飛ばす
	fp.get(input);
	fp.get(input);

	//頂点データを読み込み
	for (int i = 0; i < vertexcount_; i++)
	{
		fp >> model_[i].x >> model_[i].y >> model_[i].z;
		fp >> model_[i].tu >> model_[i].tv;
		fp >> model_[i].nx >> model_[i].ny >> model_[i].nz;
	}

	fp.close();

	return true;
}
