#include "stdafx.h"
#include "Skyplane.h"
#include "Support.h"
#include "Direct3D.h"

Skyplane::Skyplane()
{
}

Skyplane::~Skyplane()
{
}

bool Skyplane::init(const wchar_t* TextureFileName1, const wchar_t* TextureFileName2)
{
	int skyresolution;
	int texrepeat;
	float planewidth;
	float planetop;
	float planebottom;
	bool result;
	std::vector<VertexType>vertices;
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA indexdata;
	D3D11_SUBRESOURCE_DATA vertexdata;
	HRESULT hr;
	int index;
	int index1;
	int index2;
	int index3;
	int index4;

	//�p�����[�^�[�̐ݒ�
	scale_ = 0.5F;
	bright_ = 0.5F;

	//�ω��ʂ��[����
	transition_ = 0.0F;

	//�e�N�X�`���t�@�C�����������o�ɕۑ�
	wcscpy(cloudtexfilename_, TextureFileName1);
	wcscpy(perturbtexfilename_, TextureFileName2);

	//�e�p�����[�^�ݒ�
	skyresolution = 10;
	planewidth = 10.0F;
	planetop = 0.5F;
	planebottom = 0.0F;
	texrepeat = 4;

	//���ʂ��쐬
	result = initskyPlane(skyresolution, planewidth, planetop, planebottom, texrepeat);
	if (!result)
	{
		Error::showDialog("���ʂ̍쐬�Ɏ��s");
		return false;
	}

	//�o�b�t�@�̏�����
	vertexcount_ = (skyresolution + 1) * (skyresolution + 1) * 6;

	//�C���f�b�N�X�J�E���g�𒸓_���Ɠ����ɂ���
	indexcount_ = vertexcount_;

	//���_�z����쐬
	vertices.resize(vertexcount_);

	//�C���f�b�N�X�z����쐬
	indices.resize(indexcount_);

	//�C���f�b�N�X��������
	index = 0;

	//���_�ƃC���f�b�N�X�z��𕽖ʃf�[�^�ƈꏏ�Ƀ��[�h
	for (int i = 0; i < skyresolution; i++)
	{
		for (int j = 0; j < skyresolution; j++)
		{
			index1 = i * (skyresolution + 1) + j;
			index2 = i * (skyresolution + 1) + (j + 1);
			index3 = (i + 1) * (skyresolution + 1) + j;
			index4 = (i + 1) * (skyresolution + 1) + (j + 1);

			//����
			vertices[index].position = skyplane_[index1].position;
			vertices[index].texture = skyplane_[index1].texture;
			indices[index] = index;
			index++;

			//�E��
			vertices[index].position = skyplane_[index2].position;
			vertices[index].texture = skyplane_[index2].texture;
			indices[index] = index;
			index++;

			//����
			vertices[index].position = skyplane_[index3].position;
			vertices[index].texture = skyplane_[index3].texture;
			indices[index] = index;
			index++;

			//����
			vertices[index].position = skyplane_[index3].position;
			vertices[index].texture = skyplane_[index3].texture;
			indices[index] = index;
			index++;

			//�E��
			vertices[index].position = skyplane_[index2].position;
			vertices[index].texture = skyplane_[index2].texture;
			indices[index] = index;
			index++;

			//�E��
			vertices[index].position = skyplane_[index4].position;
			vertices[index].texture = skyplane_[index4].texture;
			indices[index] = index;
			index++;
		}
	}

	//���_�o�b�t�@�̐ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//�|�C���^���쐬
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

	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth = sizeof(unsigned long) * indexcount_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ւ̃|�C���^���쐬
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�z������
	indices.clear();
	vertices.clear();

	return true;
}

void Skyplane::render()
{
	//�_���ړ�
	transition_ += 0.0001F;
	if (transition_ > 1.0F)
	{
		transition_ -= 1.0F;
	}

	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	//���_�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�v���~�e�B�u�^�C�v��ݒ�
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Skyplane::destroy()
{
}

Matrix Skyplane::getWorldMatrix()
{
	world_ = Matrix::Identity;
	world_ *= Matrix::CreateScale(scale_);
	world_ *= Matrix::CreateRotationX(XMConvertToRadians(rotation_.x));
	world_ *= Matrix::CreateRotationY(XMConvertToRadians(rotation_.y));
	world_ *= Matrix::CreateRotationZ(XMConvertToRadians(rotation_.z));
	world_ *= Matrix::CreateTranslation(position_);

	return world_;
}

bool Skyplane::initskyPlane(const int PlaneResolution, const float PlaneWidth, const float PlaneTop, const float PlaneBottom, const int texRepeat)
{
	float quadsize;
	float radius;
	float constant;
	float texdelta;
	int index;
	Vector3 position;
	Vector2 texuv;

	//�z��T�C�Y��ύX
	skyplane_.resize((PlaneResolution + 1) * (PlaneResolution + 1));

	//���̊e�l�p�`�̃T�C�Y��ݒ�
	quadsize = PlaneWidth / static_cast<float>(PlaneResolution);

	//���a���v�Z
	radius = PlaneWidth / 2.0F;

	//�����̒萔���v�Z
	constant = (PlaneTop - PlaneBottom) / (radius * radius);

	//�e�N�X�`�����W���v�Z
	texdelta = static_cast<float>(texRepeat) / static_cast<float>(PlaneResolution);

	for (int i = 0; i <= PlaneResolution; i++)
	{
		position.z = (-0.5F * PlaneWidth) + (static_cast<float>(i) * quadsize);
		for (int j = 0; j <= PlaneResolution; j++)
		{
			//���_���W���v�Z
			position.x = (-0.5F * PlaneWidth) + (static_cast<float>(j) * quadsize);
			position.y = PlaneTop - (constant * ((position.x * position.x) + (position.z * position.z)));

			//�e�N�X�`�����W���v�Z
			texuv.x = static_cast<float>(j) - texdelta;
			texuv.y = static_cast<float>(i) - texdelta;

			//�C���f�b�N�X���v�Z
			index = i * (PlaneResolution + 1) + j;

			//���W��ۑ�
			skyplane_[index].position = position;
			skyplane_[index].texture = texuv;
		}
	}

	return true;
}