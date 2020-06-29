#include "ParticleSystem.h"
#include"Support.h"
#include"Direct3D.h"

ParticleSystem::ParticleSystem()
{
	//�e�p�����[�^�[�̃f�t�H���g��ݒ�
	//�o���ʒu
	data_.deviation = Vector3::Zero;
	data_.deviationrange = 0.5F;
	
	//�폜�ʒu
	data_.deleteline = 0.0F;

	//�����x��ݒ�
	data_.velocity = 1.0F;
	data_.velocityVariation = 0.2F;

	//�T�C�Y��ݒ�
	data_.size = 0.2F;

	//1�b������̕��o����ݒ�
	data_.perSecond = 25.0F;

	//�p�[�e�B�N���̍ő吔��ݒ�
	data_.max = 200;

	//�F�̊��ݒ�
	data_.colorbase = 0.5F;

	//�F�͈̔͂�ݒ�
	data_.colorrange = 0.5F;

	//�A���t�@�̊��ݒ�
	data_.alphabase = 0.5F;

	//�A���t�@�͈̔͂�ݒ�
	data_.alpharange = 0.5F;
}

ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::init(const wchar_t* FileName)
{
	bool result;

	//�t�@�C���������[�J���ɕۑ�
	wcscpy(filename_, FileName);

	//�p�[�e�B�N���V�X�e���̏�����
	result = initParticleSystem();
	if (!result)
	{
		Error::showDialog("�p�[�e�B�N���V�X�e���̏������Ɏ��s");
		return false;
	}

	return true;
}

bool ParticleSystem::update()
{
	bool result;

	//�Â��p�[�e�B�N����j��
	killParticle();

	//�V�����p�[�e�B�N������o
	emitParticle();

	//�p�[�e�B�N�����X�V
	updateParticle();

	//���I���_�o�b�t�@���X�V
	result = updateBuffer();
	if (!result)
	{
		Error::showDialog("�o�b�t�@�̍X�V�Ɏ��s");
		return false;
	}

	return true;
}

void ParticleSystem::render()
{
	//�O���t�B�b�N�X�p�C�v���C���ɔz�u
	renderBuffer();
}

void ParticleSystem::destroy()
{
	//�e�N�X�`�������
	TextureFactory::getInstance()->deleteTexture(filename_);
}

bool ParticleSystem::initParticleSystem()
{
	bool result;

	//1�b������̃p�[�e�B�N���̕��o���̗ݐώ��Ԃ�������
	accumulatedtime_ = 0.0F;

	//�o�b�t�@�̏�����
	result = initbuffer();
	if (!result)
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	return true;
}

bool ParticleSystem::initbuffer()
{
	std::vector<unsigned long> indices;
	D3D11_BUFFER_DESC vertexbufferdesc;
	D3D11_BUFFER_DESC indexbufferdesc;
	D3D11_SUBRESOURCE_DATA vertexdata;
	D3D11_SUBRESOURCE_DATA indexdata;
	HRESULT hr;

	//���_�z��̍ő吔��ݒ�
	vertexcnt_ = data_.max * 6;

	//�C���f�b�N�X�z��̍ő吔��ݒ�
	indexcnt_ = vertexcnt_;

	//�����_�����O�����p�[�e�B�N���̔z����쐬
	vertices_.resize(vertexcnt_);
	if (vertices_.empty())
	{
		Error::showDialog("���_�z��̃T�C�Y��0�ł�");
		return false;
	}

	//�C���f�b�N�X�z����쐬
	indices.resize(indexcnt_);
	if (indices.empty())
	{
		Error::showDialog("�C���f�b�N�X�z��̃T�C�Y��0�ł�");
		return false;
	}

	//�z���������
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		indices[i] = i;
	}

	//���I���_�o�b�t�@�̐ݒ�
	vertexbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexbufferdesc.ByteWidth = sizeof(VertexType) * vertexcnt_;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexbufferdesc.MiscFlags = 0;
	vertexbufferdesc.StructureByteStride = 0;

	//�T�u���\�[�X�ւ̃|�C���^���쐬
	vertexdata.pSysMem = &vertices_[0];
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�ÓI�C���f�b�N�X�o�b�t�@�̐ݒ�
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
		return false;
	}

	//�s�v�ɂȂ����̂œ�������j��
	indices.clear();

	return true;
}

bool ParticleSystem::updateBuffer()
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	VertexType* verticesptr;
	int index;

	ZeroMemory(&vertices_[0], sizeof(VertexType)*vertexcnt_);

	//�C���f�b�N�X��������
	index = 0;
	for (unsigned int i = 0;i<particlevector_.size();i++)
	{
		//����
		vertices_[index].position = Vector3(particlevector_[i].position.x - data_.size, particlevector_[i].position.y - data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(0.0F, 1.0F);
		vertices_[index].color = particlevector_[i].color;
		index++;

		//����
		vertices_[index].position = Vector3(particlevector_[i].position.x - data_.size, particlevector_[i].position.y + data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(0.0F, 0.0F);
		vertices_[index].color = particlevector_[i].color;
		index++;

		//�E��
		vertices_[index].position = Vector3(particlevector_[i].position.x + data_.size, particlevector_[i].position.y - data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(1.0F, 1.0F);
		vertices_[index].color = particlevector_[i].color;
		index++;

		//�E��
		vertices_[index].position = Vector3(particlevector_[i].position.x + data_.size, particlevector_[i].position.y - data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(1.0F, 1.0F);
		vertices_[index].color = particlevector_[i].color;
		index++;

		//����
		vertices_[index].position = Vector3(particlevector_[i].position.x - data_.size, particlevector_[i].position.y + data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(0.0F, 0.0F);
		vertices_[index].color = particlevector_[i].color;
		index++;

		//�E��
		vertices_[index].position = Vector3(particlevector_[i].position.x + data_.size, particlevector_[i].position.y + data_.size, particlevector_[i].position.z);
		vertices_[index].texture = Vector2(1.0F, 0.0F);
		vertices_[index].color = particlevector_[i].color;
		index++;
	}

	//���_�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(vertexbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("���_�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�V�F�[�_�[�ւ̃|�C���^���쐬
	verticesptr = (VertexType*)mappedresource.pData;

	//���_�o�b�t�@���R�s�[
	memcpy(verticesptr, (void*)vertices_.data(), sizeof(VertexType) * vertexcnt_);

	//���_�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(vertexbuffer_.Get(), 0);

	return true;
}

void ParticleSystem::renderBuffer()
{
	unsigned int stride;
	unsigned int offset;

	//���ƃI�t�Z�b�g��ݒ�
	stride = sizeof(VertexType);
	offset = 0;

	//���_�o�b�t�@����̓A�Z���u���ŃA�N�e�B�u�ɐݒ�
	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@����̓A�Z���u���ŃA�N�e�B�u�ɐݒ�
	Direct3D::getInstance()->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//���_�̃v���~�e�B�u�^�C�v��ݒ�
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ParticleSystem::emitParticle()
{
	bool emit;
	Vector3 position;
	Vector4 color;
	std::random_device seed;
	std::mt19937 engine(seed());
	std::normal_distribution<> colordist(data_.colorbase, data_.colorrange);
	std::normal_distribution<> xdist(data_.deviation.x, data_.deviationrange);
	std::normal_distribution<> ydist(data_.deviation.y, data_.deviationrange);
	std::normal_distribution<> zdist(data_.deviation.z, data_.deviationrange);
	std::normal_distribution<> alphadist(data_.alphabase, data_.alpharange);
	std::normal_distribution<> velodist(data_.velocity, data_.velocityVariation);
	ParticleType newparticle;

	//�t���[�����Ԃ����Z
	accumulatedtime_ += kFrameTime;

	//�t���O���I�t�ɐݒ�
	emit = true;

	////�p�[�e�B�N������o����^�C�~���O���𔻒f
	//if (accumulatedtime_ > (1000.0F / data_.perSecond))
	//{
	//	accumulatedtime_ = 0.0F;
	//	emit = true;
	//}

	//���o����p�[�e�B�N��������ꍇ�t���[�����Ƃ�1���o
	if (emit && (particlevector_.size() < data_.max))
	{
		//�����_���Ƀv���p�e�B��ݒ肵����
		newparticle.position.x = static_cast<float>(xdist(engine));
		newparticle.position.y = static_cast<float>(ydist(engine));
		newparticle.position.z = static_cast<float>(zdist(engine));
		
		//���x��ݒ�
		newparticle.velocity = data_.velocity + static_cast<float>(velodist(engine));

		//�F��ݒ�
		newparticle.color.x = static_cast<float>(colordist(engine));
		newparticle.color.y = static_cast<float>(colordist(engine));
		newparticle.color.z = static_cast<float>(colordist(engine));
		newparticle.color.w = static_cast<float>(alphadist(engine));
		
		newparticle.active = true;

		//�ݒ肵���f�[�^�𖖔��ɒǉ�
		particlevector_.push_back(newparticle);

		//Z������Ƀ\�[�g
		std::sort(particlevector_.begin(), particlevector_.end(), [](const ParticleType& First, const ParticleType& Second) {return First.position.z < Second.position.z; });
	}
}

void ParticleSystem::updateParticle()
{
	for (auto& itr : particlevector_)
	{
		itr.position.y -= itr.velocity * kFrameTime * kPositionAttenuation;
		itr.color.w -= kFrameTime * kAlphaAttenuation;
	}
}

void ParticleSystem::killParticle()
{
	for (auto itr = particlevector_.begin(); itr != particlevector_.end();)
	{
		if (itr->active && (itr->position.y<data_.deleteline||itr->color.w<=0.0F))
		{
			itr->active = false;
			itr = particlevector_.erase(itr);
		}
		else
			itr++;
	}
}
