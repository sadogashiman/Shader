#include "stdafx.h"
#include "model.h"
#include"Direct3D.h"
#include"TextureFactory.h"
bool Model::initbuff()
{
	Vertextype* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexbuffdesc;
	D3D11_BUFFER_DESC indexbuffdesc;
	D3D11_SUBRESOURCE_DATA vertexdata, indexdata;
	HRESULT hr;

	//���_�z����쐬
	vertices = new Vertextype[vertexcount_];
	if (!vertices)
	{
		return false;
	}

	//�C���f�b�N�X�z��̍쐬
	indices = new unsigned long[indexcount_];
	if (!indices)
	{
		return false;
	}

	//���_�z��Ƀf�[�^�����[�h
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = Vector3(model_[i].x, model_[i].y, model_[i].z);
		vertices[i].texture = Vector2(model_[i].tu, model_[i].tv);
		vertices[i].normal = Vector3(model_[i].nx, model_[i].ny, model_[i].nz);

		indices[i] = i;
	}

	//�ÓI���_�o�b�t�@�̐ݒ�
	vertexbuffdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbuffdesc.ByteWidth = sizeof(Vertextype) * vertexcount_;
	vertexbuffdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbuffdesc.CPUAccessFlags = 0;
	vertexbuffdesc.MiscFlags = 0;
	vertexbuffdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ɒ��_�f�[�^�ւ̃|�C���^�[��^����
	//ZeroMemory(&vertexdata, sizeof(vertexdata));
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, &vertexbuff_);
	if (FAILED(hr))
	{
		return false;
	}

	//�ÓI�C���f�b�N�X�o�b�t�@�̐ݒ�
	indexbuffdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbuffdesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexbuffdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbuffdesc.CPUAccessFlags = 0;
	indexbuffdesc.MiscFlags = 0;
	indexbuffdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ɃC���f�b�N�X�f�[�^�ւ̃|�C���^��^����
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbuffdesc, &indexdata, &indexbuff_);
	if (FAILED(hr))
	{
		return false;
	}

	//�z������
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void Model::destroybuff()
{
	//�C���f�b�N�X�o�b�t�@�����
	if (indexbuff_)
	{
		indexbuff_->Release();
		indexbuff_ = nullptr;
	}

	//���_�o�b�t�@�̉��
	if (vertexbuff_)
	{
		vertexbuff_->Release();
		vertexbuff_ = nullptr;
	}
}

void Model::renderbuff()
{
	unsigned int stride;
	unsigned int offset;

	//���_�o�b�t�@�̃X�g���C�h�ƃI�t�Z�b�g��ݒ�
	stride = sizeof(Vertextype);
	offset = 0;

	//���̓A�Z���u���Œ��_�o�b�t�@���A�N�e�B�u�ɂ��ă����_�����O�\��
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, &vertexbuff_, &stride, &offset);

	//���̓A�Z���u���ŃC���f�b�N�X�o�b�t�@���A�N�e�B�u�ɂ��ă����_�����O�\��
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuff_, DXGI_FORMAT_R32_UINT, 0);

	//���_�o�b�t�@���烌���_�����O�����v���~�e�B�u�̃^�C�v
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::Loadtexture(const wchar_t* FileName)
{
	memcpy(filename_, FileName, sizeof(wchar_t*));
	texture_ = TextureFactory::getInstance()->getTexture(FileName);

	return true;
}

void Model::Releasetexture()
{
	TextureFactory::getInstance()->deleteTexture(filename_);
}

bool Model::LoadModel(const wchar_t* FileName)
{
	std::ifstream fin;
	char input;

	//�t�@�C���W�J
	fin.open(FileName);

	//�W�J���s
	if (fin.fail())
	{
		return false;
	}

	//���_�̐����擾
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//���_�J�E���g��ǂݎ��
	fin >> vertexcount_;

	//�C���f�b�N�X�̐��𒸓_�̐��Ɠ����ɐݒ�
	indexcount_ = vertexcount_;

	//�ǂݍ��܂ꂽ���_�����g�p���Ă��ł��`��
	model_ = new ModelType[vertexcount_];
	if (!model_)
	{
		return false;
	}

	//�f�[�^��擪�܂œǂݎ��
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	//���_�f�[�^��ǂݍ���
	for (int i = 0; i < vertexcount_; i++)
	{
		fin >> model_[i].x >> model_[i].y >> model_[i].z;
		fin >> model_[i].tu >> model_[i].tv;
		fin >> model_[i].nx >> model_[i].ny >> model_[i].nz;
	}

	fin.close();

	return true;
}

void Model::ReleaseModel()
{
	if (model_)
	{
		delete model_;
		model_ = nullptr;
	}
}

Model::Model()
{
	vertexbuff_ = nullptr;
	indexbuff_ = nullptr;
	model_ = nullptr;
}

Model::~Model()
{
}

bool Model::init(const wchar_t* TextureFileName, const wchar_t* ModelFileName)
{
	bool result;
	//���f���f�[�^�ǂݍ���
	result = LoadModel(ModelFileName);
	if (!result)
	{
		return false;
	}

	//���_�y�уC���f�b�N�X�o�b�t�@��������
	result = initbuff();

	//���_�ƃC���f�b�N�X�o�b�t�@��������
	if (!result)
	{
		return false;
	}

	//���̃��f���Ŏg�p����e�N�X�`�������[�h
	result = Loadtexture(TextureFileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void Model::destroy()
{
	//���f���e�N�X�`�������
	Releasetexture();

	//���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@��j��
	destroybuff();

	//���f�����
	ReleaseModel();
}

void Model::render()
{
	//�O���t�B�b�N�X�p�C�v���C���ɒ��_�o�b�t�@���C���f�b�N�X�o�b�t�@��z�u
	renderbuff();
}

void Model::setPosition(const float X, const float Y, const float Z)
{
	positionx = X;
	positiony = Y;
	positionz = Z;
}

void Model::getPosition(float& X, float& Y, float& Z)
{
	X = positionx;
	Y = positiony;
	Z = positionz;
}
