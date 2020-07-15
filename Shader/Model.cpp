#include "stdafx.h"
#include "model.h"
#include"Support.h"
#include"Texture.h"



Model::Model()
{
	ZeroMemory(&texturefilename_, sizeof(texturefilename_));
	ZeroMemory(&mapfilename_, sizeof(mapfilename_));
	scale_ = 1.0F;
	world_ = XMMatrixIdentity();
	instanceptr_ = Direct3D::getInstance();
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

	//�t�@�C������ۑ�
	wcscpy(texturefilename_[0], TextureFileName);

	if (!initBuffer())
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	return true;
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2)
{
	//���f���f�[�^�ǂݍ���
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//�t�@�C������ۑ�
	wcscpy(texturefilename_[0], TextureFileName1);
	wcscpy(texturefilename_[1], TextureFileName2);

	if (!initBuffer())
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	return true;
}

bool Model::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2, const wchar_t* MapFileName)
{
	//���f���f�[�^�ǂݍ���
	if (!loadModel(ModelFileName))
	{
		return false;
	}

	//�t�@�C������ۑ�
	wcscpy(texturefilename_[0], TextureFileName1);
	wcscpy(texturefilename_[1], TextureFileName2);
	wcscpy(mapfilename_, MapFileName);

	if (!initBuffer())
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	return true;
}

void Model::destroy()
{
	//���̃��f�����g�p�����e�N�X�`�������������
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
	//�O���t�B�b�N�X�p�C�v���C���ɒ��_�o�b�t�@���C���f�b�N�X�o�b�t�@��z�u
	renderBuffer();
}

Matrix Model::getWorldMatrix()
{
	world_ =  Matrix::Identity;
	world_ *= Matrix::CreateScale(scale_);
	world_ *= Matrix::CreateRotationX(XMConvertToRadians(rotation_.x));
	world_ *= Matrix::CreateRotationY(XMConvertToRadians(rotation_.y));
	world_ *= Matrix::CreateRotationZ(XMConvertToRadians(rotation_.z));
	world_ *= Matrix::CreateTranslation(position_);

	return world_;
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
	hr = instanceptr_->getDevice()->CreateBuffer(&vertexbuffdesc, &vertexdata, vertexbuff_.GetAddressOf());
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
	hr = instanceptr_->getDevice()->CreateBuffer(&indexbuffdesc, &indexdata, indexbuff_.GetAddressOf());
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
	instanceptr_->getContext()->IASetVertexBuffers(0, 1, vertexbuff_.GetAddressOf(), &stride, &offset);

	//���̓A�Z���u���ŃC���f�b�N�X�o�b�t�@���A�N�e�B�u�ɂ��ă����_�����O�\��
	instanceptr_->getContext()->IASetIndexBuffer(indexbuff_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//���_�o�b�t�@���烌���_�����O�����v���~�e�B�u�̃^�C�v
	instanceptr_->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::loadModel(const wchar_t* ModelFileName)
{
	std::fstream fs;
	char input;
	FILE* fp;
	char tmp[MAX_PATH];
	char buffer[256];

	//�t�@�C���p�X���L�����m�F
	if (!Support::searchFile(ModelFileName))
	{
		return false;
	}

	//�t�@�C���W�J�̂��߂ɕ�����̌^�ϊ�
	wcstombs(tmp, ModelFileName,MAX_PATH);

	//�t�@�C���W�J
	fp = fopen(tmp,"r");
	if (fp == nullptr)
		return false;

	//�t�@�C���ǂݍ���
	input = fgetc(fp);

	//���_���܂Ŕ�΂�
	while (input != ':')
	{
		input = fgetc(fp);
	}

	//���_�����擾
	fscanf(fp, "%d", &vertexcount_);

	//�C���f�b�N�X����ݒ�
	indexcount_ = vertexcount_;

	//�z��T�C�Y��ύX
	model_.resize(vertexcount_);

	input = fgetc(fp);

	//�擪�܂œǂݔ�΂�
	while (input != ':')
	{
		input = fgetc(fp);
	}

	//���s���΂�
	fgets(buffer, 256, fp);

	//���f���p�����[�^�[�擾
	for (int i = 0; i < vertexcount_; i++)
	{
		//���W���擾
		fscanf(fp, "%f %f %f",&model_[i].x, &model_[i].y, &model_[i].z);

		//�e�N�X�`��UV���擾
		fscanf(fp, "%f %f", &model_[i].tu, &model_[i].tv);

		//�@�����擾
		fscanf(fp, "%f %f %f", &model_[i].nx, &model_[i].ny, &model_[i].nz);
	}

	//�t�@�C���N���[�Y
	fclose(fp);

	return true;
}
