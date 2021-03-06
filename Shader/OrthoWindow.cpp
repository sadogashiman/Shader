#include "stdafx.h"
#include "OrthoWindow.h"

OrthoWindow::OrthoWindow()
{
	ZeroMemory(this, sizeof(OrthoWindow));
}

OrthoWindow::~OrthoWindow()
{
}

bool OrthoWindow::init(const float WindowWidth, const float WindowHeight)
{
	float left, right, top, bottom;
	std::vector<VertexType> vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//ウィンドウ座標を計算
	left = (WindowWidth / 2) * -1;
	right = left + WindowWidth;
	top = WindowHeight / 2;
	bottom = top - WindowHeight;

	//頂点の数を設定
	vertexcnt_ = 6;

	//インデックスの数を設定
	indexcnt_ = vertexcnt_;

	//頂点配列を作成
	vertices.resize(vertexcnt_);
	if (vertices.empty())
	{
		return false;
	}

	//インデックス配列を作成
	indices.resize(indexcnt_);
	if (indices.empty())
	{
		return false;
	}

	//配列にデータをロード
	vertices[0].position = Vector3(left, top, 0.0F);
	vertices[0].texture = Vector2(0.0F, 0.0F);

	vertices[1].position = Vector3(right, bottom, 0.0F);
	vertices[1].texture = Vector2(1.0F, 1.0F);

	vertices[2].position = Vector3(left, bottom, 0.0F);
	vertices[2].texture = Vector2(0.0F, 1.0F);

	vertices[3].position = Vector3(left, top, 0.0F);
	vertices[3].texture = Vector2(0.0F, 0.0F);

	vertices[4].position = Vector3(right, top, 0.0F);
	vertices[4].texture = Vector2(1.0F, 0.0F);

	vertices[5].position = Vector3(right, bottom, 0.0F);
	vertices[5].texture = Vector2(1.0F, 1.0F);

	for (int i = 0; i < indexcnt_; i++)
	{
		indices[i] = i;
	}

	//頂点バッファを設定
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//サブリソースにポインターを渡す
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//頂点バッファを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//インデックスバッファを設定
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcnt_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	hr = instanceptr_->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//破棄
	vertices.clear();
	indices.clear();

	return true;
}

void OrthoWindow::render()
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	instanceptr_->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	instanceptr_->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	instanceptr_->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
