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

	//�e���C���O���b�h�̃T�C�Y��ݒ�
	terrainwidth = 256;
	terrainheight = 256;

	//�O���b�h�̐F��ݒ�
	color = Vector4::One;

	//���_�����v�Z
	vertexcnt_ = (terrainwidth - 1) * (terrainheight - 1) * 8;

	//�C���f�b�N�X����ݒ�
	indexcnt_ = vertexcnt_;

	//���_�z��𐶐�
	vertices.resize(vertexcnt_);

	//�C���f�b�N�X�z����쐬
	indices.resize(indexcnt_);

	//�C���f�b�N�X�z��ƒ��_�z���������
	index = 0;
	int width = terrainwidth - 1;
	int height = terrainheight - 1;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			//���C��1
			//����
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//�E��
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//���C��2
			//�E��
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//�E��
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//���C��3
			//�E��
			position.x = static_cast<float>(i + 1);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//����
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//���C��4
			//����
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;

			//����
			position.x = static_cast<float>(i);
			position.z = static_cast<float>(j + 1);

			vertices[index].position = Vector3(position.x, 0.0F, position.z);
			vertices[index].color = color;
			indices[index] = index;
			index++;
		}
	}

	//���_�o�b�t�@��ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ւ̃|�C���^���쐬
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�C���f�b�N�X�o�b�t�@��ݒ�
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcnt_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ւ̃|�C���^���쐬
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	return true;
}

void Terrain::render()
{
	unsigned int stride;
	unsigned int offset;

	//���ƃI�t�Z�b�g��ݒ�
	stride = sizeof(VertexType);
	offset = 0;

	//���_�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�v���~�e�B�u�̃��[�h��ݒ�
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Terrain::destroy()
{
}
