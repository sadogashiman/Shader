#include "stdafx.h"
#include "SkyDom.h"
#include "Direct3D.h"

bool SkyDom::loadSkyDomModel(const wchar_t* ModelFileName)
{
	if (!Support::searchFile(ModelFileName))
	{
		Error::showDialog("スカイドームモデルへのファイルパスが無効です");
		return false;
	}

	std::ifstream fin;
	char input;
	//objファイル展開
	fin.open(ModelFileName);
	if (fin.fail())
	{
		Error::showDialog("ファイル展開に失敗");
		return false;
	}

	//頂点数まで読み取り
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//頂点数読み込み
	fin >> vertexcount_;

	//インデックスの数を頂点と同じに設定
	indexcount_ = vertexcount_;

	//モデル情報を格納するvectorのサイズを変更
	model_.resize(vertexcount_);
	if (model_.empty())
	{
		Error::showDialog("モデル情報を格納できませんでした");
		return false;
	}

	//座標データの先頭まで飛ばす
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin.get(input);
	fin.get(input);

	//モデルデータを読み込み
	for (auto itr : model_)
	{
		fin >> itr.position.x >> itr.position.y >> itr.position.z;
		fin >> itr.tex.x >> itr.tex.y;
		fin >> itr.normal.x >> itr.normal.y >> itr.normal.z;
	}

	//ファイルを閉じる
	fin.close();

	return true;
}

void SkyDom::releaseSkyDomModel()
{
	model_.clear();
}

bool SkyDom::init(const wchar_t* ModelFileName)
{
	bool result;

	//モデルデータ読み込み
	result = loadSkyDomModel(ModelFileName);
	if (!result)
	{
		Error::showDialog("スカイドームモデルの読み込みに失敗");
		return false;
	}
	
	//バッファ初期化
	VertexType* vertices;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//頂点配列を作成
	vertices = new VertexType[vertexcount_];
	if (!vertices)
	{
		Error::showDialog("頂点配列のメモリ確保に失敗");
		return false;
	}


	//インデックス配列を作成
	indices = new unsigned int[indexcount_];
	if (!indices)
	{
		Error::showDialog("インデックス配列のメモリ確保に失敗");
		return false;
	}

	//頂点配列とインデックス配列にデータをロード
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = model_[i].position;
		indices[i] = i;
	}

	//頂点バッファの設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//サブリソースに頂点バッファのポインタをセット
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファの作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点バッファの作成に失敗");
		return false;
	}

	//インデックスバッファの設定
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.BindFlags = sizeof(unsigned int) * indexcount_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;

	//サブリソースにインデックスバッファのポインタをセット
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("インデックスバッファの作成に失敗");
		return false;
	}

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	return true;
}

bool SkyDom::render()
{
	unsigned int stride;
	unsigned int offset;

	//ストライドとオフセットをセット
	stride = sizeof(VertexType);
	offset = 0;

	//頂点バッファをアクティブに設定
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//インデックスバッファをアクティブに設定
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//プリミティブタイプを設定
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void SkyDom::destroy()
{
	releaseSkyDomModel();
}
