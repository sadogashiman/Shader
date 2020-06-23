#include "stdafx.h"
#include "Tree.h"
#include "Direct3D.h"
Tree::Tree()
{
}

Tree::~Tree()
{
}

bool Tree::init(const wchar_t* TrunkModelFileName, const wchar_t* LeafModelFileName, float Scale)
{
	bool result;

	//�T�|�[�g�N���X�𐶐�
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	//���f���̓ǂݍ���
	result = loadModel(trunkfilename_);
	if (!result)
	{
		Error::showDialog("���̃��f���̓ǂݍ��݂Ɏ��s");
		return false;
	}

	//�C���f�b�N�X�J�E���g���R�s�[
	trunkindexcount_ = indexcount_;

	//�؂̊��̃o�b�t�@��������
	result = initTrunkBuffer();
	if (!result)
	{
		Error::showDialog("�؂̊��̃o�b�t�@�̏������Ɏ��s");
		return false;
	}

	//���f�����o�b�t�@�ɓǂݍ��܂ꂽ�̂ŉ��
	releaseModel();

	//�t���ς̃��f����ǂݍ���
	result = initLeafBuffer();
	if (!result)
	{
		Error::showDialog("�t���σ��f���̓ǂݍ��݂Ɏ��s");
		return false;
	}

	//�C���f�b�N�X�J�E���g���R�s�[
	leafindexcount_ = indexcount_;

	//�t���ς̃o�b�t�@��������
	result = initLeafBuffer();
	if (!result)
	{
		Error::showDialog("�t���ς̃o�b�t�@�̏������Ɏ��s");
		return false;
	}

	//�o�b�t�@�ɓǂݍ��܂ꂽ�̂Ń��f�������
	releaseModel();

	//���f���t�@�C��������g���q�Ⴂ�̉摜�t�@�C�����𐶐�
	wcscpy(trunkfilename_, support_.get()->renameToJPEG(TrunkModelFileName));
	wcscpy(leaffilename_, support_.get()->renameToJPEG(LeafModelFileName));

	return true;
}

void Tree::renderTrunk()
{
	//���������_�����O
	renderTrunkBuffer();
}

void Tree::renderLeaves()
{
	//�t�������_�����O
	renderLeavesBuffer();
}

void Tree::destroy()
{
	//���̃N���X���Ŏg�p�����e�f�[�^�̔j��
	destroyBuffer();
	releaseTexture();
	releaseModel();
}

bool Tree::initTrunkBuffer()
{
	std::vector<VertexType> vertices;
	std::vector<unsigned long>indices;
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
		vertices[i].position = Vector3(model_[i].position.x * scale_, model_[i].position.y * scale_, model_[i].position.z * scale_);
		vertices[i].texture = model_[i].texpos;
		vertices[i].normal = model_[i].normalpos;

		indices[i] = i;
	}

	//���_�o�b�t�@�̐ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType)*vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;
	
	//�|�C���^���쐬
	vertexdata.pSysMem = &vertices[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, trunkvertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�C���f�b�N�X�o�b�t�@�̐ݒ�
	indexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	indexbufferdesc.ByteWidth= sizeof(unsigned long) * indexcount_;
	indexbufferdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexbufferdesc.CPUAccessFlags = 0;
	indexbufferdesc.MiscFlags = 0;
	indexbufferdesc.StructureByteStride = 0;

	//�C���f�b�N�X�o�b�t�@�̃|�C���^���쐬
	indexdata.pSysMem = &indices[0];
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, trunkindexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�s�v�ȃf�[�^�̍폜
	vertices.clear();
	indices.clear();

	return true;
}

bool Tree::initLeafBuffer()
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//���_�z����쐬
	vertices = new VertexType[vertexcount_];
	if (!vertices)
	{
		Error::showDialog("���_�z��̃������m�ۂɎ��s");
		return false;
	}

	//�C���f�b�N�X�z����쐬
	indices = new unsigned long[indexcount_];
	if (!indices)
	{
		Error::showDialog("�C���f�b�N�X�z��̃������m�ۂɎ��s");
		return false;
	}

	//���_�z��ƃC���f�b�N�X�z��Ƀf�[�^�����[�h
	for (int i = 0; i < vertexcount_; i++)
	{
		vertices[i].position = Vector3(model_[i].position.x * scale_, model_[i].position.y * scale_, model_[i].position.z * scale_);
		vertices[i].texture = model_[i].texpos;
		vertices[i].normal = model_[i].normalpos;

		indices[i] = i;
	}

	//���_�o�b�t�@�̐ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcount_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = 0;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//�|�C���^���쐬
	vertexdata.pSysMem = vertices;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, leafvertexbuffer_.GetAddressOf());
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

	//�C���f�b�N�X�o�b�t�@�̃|�C���^���쐬
	indexdata.pSysMem = indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, leafindexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�s�v�ȃf�[�^�̍폜
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;
	return true;
}

void Tree::renderTrunkBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//���ƃI�t�Z�b�g��ݒ�
	stride = sizeof(VertexType);
	offset = 0;

	//���_�o�b�t�@�������_�����O�ł���悤�ɃZ�b�g
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, trunkindexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@�������_�����O�ł���悤�ɃZ�b�g
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(trunkindexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�����_�����O�̃^�C�v��ݒ�	
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Tree::renderLeavesBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//���ƃI�t�Z�b�g��ݒ�
	stride = sizeof(VertexType);
	offset = 0;

	//���_�o�b�t�@�������_�����O�ł���悤�ɃZ�b�g
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, leafindexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@�������_�����O�ł���悤�ɃZ�b�g
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(leafindexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�����_�����O�̃^�C�v��ݒ�	
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool Tree::loadTexture()
{

	return true;
}

void Tree::releaseTexture()
{
	//���̃N���X�Ŏg�p�����摜�t�@�C����j��
	TextureFactory::getInstance()->deleteTexture(trunkfilename_);
	TextureFactory::getInstance()->deleteTexture(leaffilename_);
}

bool Tree::loadModel(const wchar_t* ModelName)
{
	std::fstream fp;
	char input;

	//�t�@�C���p�X���L�����m�F
	if (!Support::searchFile(ModelName))
	{
		return false;
	}

	//�t�@�C���p�X���L���Ȃ̂��m�F�����̂œW�J
	fp.open(ModelName, std::ios::beg | std::ios::in);

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

	//�ǂݍ��܂ꂽ���_�����g�p���Ă��ł��`��
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
		fp >> model_[i].position.x >> model_[i].position.y >> model_[i].position.z;
		fp >> model_[i].texpos.x >> model_[i].texpos.y;
		fp >> model_[i].normalpos.x >> model_[i].normalpos.y >> model_[i].normalpos.z;
	}

	fp.close();
	return true;
}

void Tree::releaseModel()
{
	//���f���f�[�^��j��
	model_.clear();
}

void Tree::destroyBuffer()
{
}
