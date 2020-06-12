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
	HRESULT hr;
	std::vector<VertexType>vertices;
	std::vector<unsigned long> indices;
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
	vertices.resize(vertexcnt_);

	//インデックス配列を作成
	indices.resize(indexcnt_);

	//インデックス配列と頂点配列を初期化
	index = 0;
	int width = terrainwidth - 1;
	int height = terrainheight - 1;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			//ライン1
			//左上
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//右上
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//ライン2
			//右上
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//右下
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//ライン3
			//右下
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//左下
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//ライン4
			//左下
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//左上
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
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
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Terrain::destroy()
{
}
