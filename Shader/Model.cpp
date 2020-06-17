#include "stdafx.h"
#include "model.h"
#include"Direct3D.h"
#include"Support.h"
#include"Texture.h"



Model::Model()
{
}

Model::~Model()
{
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName)
{
	//���f���f�[�^�ǂݍ���
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//���f���t�@�C���̊g���q��ύX
	wcscpy(texturefilename_, Support::renameExtension(ModelFileName,"jpeg"));

	//�e�N�X�`���ǂݍ���
	loadTexture(texturefilename_);

	if (!initBuffer())
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	return true;
}

void Model::destroy()
{
	//���f���e�N�X�`�������
	releaseTexture();
}

void Model::render()
{
	//�O���t�B�b�N�X�p�C�v���C���ɒ��_�o�b�t�@���C���f�b�N�X�o�b�t�@��z�u
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

	//���_�z����쐬
	vertices.resize(vertexcount_);

	//�C���f�b�N�X�z��̍쐬
	indices.resize(indexcount_);

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
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, vertexbuff_.GetAddressOf());
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
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@���쐬
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

	//���_�o�b�t�@�̃X�g���C�h�ƃI�t�Z�b�g��ݒ�
	stride = sizeof(Vertextype);
	offset = 0;

	//���̓A�Z���u���Œ��_�o�b�t�@���A�N�e�B�u�ɂ��ă����_�����O�\��
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuff_.GetAddressOf(), &stride, &offset);

	//���̓A�Z���u���ŃC���f�b�N�X�o�b�t�@���A�N�e�B�u�ɂ��ă����_�����O�\��
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuff_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//���_�o�b�t�@���烌���_�����O�����v���~�e�B�u�̃^�C�v
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::loadTexture(const wchar_t* FileName)
{

	TextureFactory::getInstance()->getTexture(FileName);
}

bool Model::loadModel(const wchar_t* ModelFileName)
{
	std::fstream fp;
	char input;

	//�t�@�C���p�X���L�����m�F
	if (!Support::searchFile(ModelFileName))
	{
		return false;
	}

	//�t�@�C���p�X���L���Ȃ̂��m�F�����̂œW�J
	fp.open(ModelFileName, std::ios::beg | std::ios::in);

	if (fp.fail())
	{
		return false;
	}

	//���_�̐����擾
	fp.get(input);
	while (input != ':')
	{
		fp.get(input);
	}

	//���_�J�E���g��ǂݎ��
	fp >> vertexcount_;

	//�C���f�b�N�X�̐��𒸓_�̐��Ɠ����ɐݒ�
	indexcount_ = vertexcount_;

	//�ǂݍ��܂ꂽ���_�����g�p���ă��f���z����쐬
	model_.resize(vertexcount_);

	//�f�[�^��擪�܂œǂݎ��
	fp.get(input);
	while (input != ':')
	{
		fp.get(input);
	}

	//���s���΂�
	fp.get(input);
	fp.get(input);

	//���_�f�[�^��ǂݍ���
	for (int i = 0; i < vertexcount_; i++)
	{
		fp >> model_[i].x >> model_[i].y >> model_[i].z;
		fp >> model_[i].tu >> model_[i].tv;
		fp >> model_[i].nx >> model_[i].ny >> model_[i].nz;
	}

	fp.close();

	return true;
}

void Model::releaseTexture()
{
	TextureFactory::getInstance()->deleteTexture(texturefilename_);
}

void Model::setPosition(const float X, const float Y, const float Z)
{
	positionx_ = X;
	positiony_ = Y;
	positionz_ = Z;
}

void Model::getPosition(float& X, float& Y, float& Z)
{
	X = positionx_;
	Y = positiony_;
	Z = positionz_;
}
