#include "stdafx.h"
#include "SkyDome.h"
#include "Direct3D.h"
#include"Support.h"


SkyDome::SkyDome()
{
}

SkyDome::~SkyDome()
{
}

bool SkyDome::init(const wchar_t* ModelFileName)
{
	bool result;

	//���f���f�[�^�ǂݍ���
	result = loadSkyDomModel(ModelFileName);
	if (!result)
	{
		Error::showDialog("�X�J�C�h�[�����f���̓ǂݍ��݂Ɏ��s");
		return false;
	}

	//�o�b�t�@������
	std::vector<VertexType>vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//���_�z����쐬
	vertices.resize(vertexcount_);

	//�C���f�b�N�X�z����쐬
	indices.resize(indexcount_);

	//���_�z��ƃC���f�b�N�X�z��Ƀf�[�^�����[�h
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = model_[i].position;
		indices[i] = i;
	}

	//���_�o�b�t�@�̐ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ɒ��_�o�b�t�@�̃|�C���^���Z�b�g
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ɃC���f�b�N�X�o�b�t�@�̃|�C���^���Z�b�g
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

void SkyDome::render()
{
	unsigned int stride;
	unsigned int offset;

	//�X�g���C�h�ƃI�t�Z�b�g���Z�b�g
	stride = sizeof(VertexType);
	offset = 0;

	//���_�o�b�t�@���A�N�e�B�u�ɐݒ�
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@���A�N�e�B�u�ɐݒ�
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�v���~�e�B�u�^�C�v��ݒ�
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void SkyDome::destroy()
{
	releaseSkyDomModel();
}

bool SkyDome::loadSkyDomModel(const wchar_t* ModelFileName)
{
	if (!Support::searchFile(ModelFileName))
	{
		Error::showDialog("�X�J�C�h�[�����f���ւ̃t�@�C���p�X�������ł�");
		return false;
	}

	std::ifstream fin;
	char input;
	//obj�t�@�C���W�J
	fin.open(ModelFileName);
	if (fin.fail())
	{
		Error::showDialog("�t�@�C���W�J�Ɏ��s");
		return false;
	}

	//���_���܂œǂݎ��
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//���_���ǂݍ���
	fin >> vertexcount_;

	//�C���f�b�N�X�̐��𒸓_�Ɠ����ɐݒ�
	indexcount_ = vertexcount_;

	//���f�������i�[����vector�̃T�C�Y��ύX
	model_.resize(vertexcount_);
	if (model_.empty())
	{
		Error::showDialog("���f�������i�[�ł��܂���ł���");
		return false;
	}

	//���W�f�[�^�̐擪�܂Ŕ�΂�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	fin.get(input);
	fin.get(input);

	//���f���f�[�^��ǂݍ���
	for (auto& itr : model_)
	{
		fin >> itr.position.x >> itr.position.y >> itr.position.z;
		fin >> itr.tex.x >> itr.tex.y;
		fin >> itr.normal.x >> itr.normal.y >> itr.normal.z;
	}

	//�t�@�C�������
	fin.close();

	return true;
}

void SkyDome::releaseSkyDomModel()
{
	model_.clear();
}
