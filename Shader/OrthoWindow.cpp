#include "stdafx.h"
#include "OrthoWindow.h"
#include"Direct3D.h"

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
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//�E�B���h�E���W���v�Z
	left = (WindowWidth / 2) * -1;
	right = left + WindowWidth;
	top = WindowHeight / 2;
	bottom = top - WindowHeight;

	//���_�̐���ݒ�
	vertexcnt_ = 6;

	//�C���f�b�N�X�̐���ݒ�
	indexcnt_ = vertexcnt_;

	//���_�z����쐬
	vertices = new VertexType[vertexcnt_];
	if (!vertices)
	{
		return false;
	}

	//�C���f�b�N�X�z����쐬
	indices = new unsigned long[indexcnt_];
	if (!indices)
	{
		return false;
	}

	//�z��Ƀf�[�^�����[�h
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

	//���_�o�b�t�@��ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�Ƀ|�C���^�[��n��
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�C���f�b�N�X�o�b�t�@��ݒ�
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcnt_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�j��
	delete[] indices;
	indices = nullptr;

	delete[] vertices;
	vertices = nullptr;

	return true;
}

void OrthoWindow::render()
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
