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

bool Terrain::init()
{
	HRESULT hr;
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	int terrainwidth;
	int terrainheight;
	int index;
	Vector4 color;
	Vector3 position;

	//テレイングリッドのサイズを設定
	terrainwidth = 256;
	terrainheight = 256;

	//グリッドの色を設定
	color = Vector4::One;

	//頂点数を計算
	vertexcnt_ = (terrainwidth - 1) * (terrainheight - 1) * 8;

	//インデックス数を設定
	indexcnt_ = vertexcnt_;

	//頂点配列を生成
	vertices = new VertexType[vertexcnt_];
	if (!vertices)
	{
		Error::showDialog("頂点配列のメモリ確保に失敗");
		return false;
	}

	//インデックス配列を作成
	indices = new unsigned long[indexcnt_];
	if (!indices)
	{
		Error::showDialog("インデックス配列のメモリ確保に失敗");
		return false;
	}

	//インデックス配列と頂点配列を初期化
	index = 0;

	for (int i = 0; i < (terrainwidth - 1); i++)
	{
		for (int j = 0; j < (terrainheight - 1); i++)
		{
			//ライン1
			//左上
			position.x = static_cast<float>(i);
			position.y = static_cast<float>(j + 1);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//右上
			position.x = static_cast<float>(i + 1);
			position.y = static_cast<float>(j + 1);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//ライン2
			//右上
			position.x = static_cast<float>(i + 1);
			position.y = static_cast<float>(j + 1);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//右下
			position.x = static_cast<float>(i + 1);
			position.y = static_cast<float>(j);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//ライン3
			//右下
			position.x = static_cast<float>(i + 1);
			position.y = static_cast<float>(j);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//左下
			position.x = static_cast<float>(i);
			position.y = static_cast<float>(j + 1);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//ライン4
			//左下
			position.x = static_cast<float>(i);
			position.y = static_cast<float>(j);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//左上
			position.x = static_cast<float>(i);
			position.y = static_cast<float>(j + 1);

			vertices[index].position = static_cast<Vector3>(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;
		}
	}

	//頂点バッファを設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//サブリソースへのポインタを作成
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, &vertexbuffer_);
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
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//インデックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, &indexbuffer_);
	if (FAILED(hr))
	{
		Error::showDialog("インデックスバッファの作成に失敗");
		return false;
	}

	delete[] indices;
	indices = nullptr;

	delete[] vertices;
	vertices = nullptr;

	return true;
}

bool Terrain::render()
{
	unsigned int stride;
	unsigned int offset;

	//幅とオフセットを設定
	stride = sizeof(VertexType);
	offset = 0;

	//頂点バッファをセット
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, &vertexbuffer_, &stride, &offset);

	//インデックスバッファをセット
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_, DXGI_FORMAT_R32_UINT, 0);

	//プリミティブのモードを設定
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);


	return false;
}

void Terrain::destroy()
{
	SAFE_RELEASE(indexbuffer_);
	SAFE_RELEASE(vertexbuffer_);
}
