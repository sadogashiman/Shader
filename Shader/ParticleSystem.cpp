#include "ParticleSystem.h"
#include"Support.h"
#include"Direct3D.h"

ParticleSystem::ParticleSystem()
{
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

	//�o�b�t�@�̏�����
	result = initbuffer();
	if (!result)
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	return true;
}

bool ParticleSystem::update(const float FrameTime)
{
	bool result;

	//�Â��p�[�e�B�N����j��
	killParticle();

	//�V�����p�[�e�B�N������o
	emitParticle(FrameTime);

	//�p�[�e�B�N�����X�V
	updateParticle(FrameTime);

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
	//�o�b�t�@��j��
	destroyBuffer();

	//�p�[�e�B�N���V�X�e�����~
	destroyParticleSystem();

	//�e�N�X�`�������
	TextureFactory::getInstance()->deleteTexture(filename_);
}

bool ParticleSystem::initParticleSystem()
{

	//���o�ʒu��ݒ�
	data_.deviation.x = 0.5F;
	data_.deviation.y = 0.1F;
	data_.deviation.z = 2.0F;

	//�����x��ݒ�
	data_.velocity = 1.0F;
	data_.velocityVariation = 0.2F;

	//�T�C�Y��ݒ�
	data_.size = 0.2F;

	//1�b������̕��o����ݒ�
	data_.perSecond = 250.0F;

	//�p�[�e�B�N���̍ő吔��ݒ�
	data_.max = 5000;

	//���X�g���쐬
	particlelist_.resize(data_.max);

	std::list<std::unique_ptr<ParticleType>>::iterator itr;

	//���X�g���̃p�[�e�B�N���̏�Ԃ�������
	for (itr = particlelist_.begin(); itr != particlelist_.end(); itr++)
	{
		(*itr)->active = false;
	}

	//���ݕ��o����Ă���p�[�e�B�N���̐���0�ɂ���
	currentcnt_ = 0;

	//1�b������̃p�[�e�B�N���̕��o���̗ݐώ��Ԃ�������
	accumulatedtime_ = 0.0F;

	return true;
}

void ParticleSystem::destroyParticleSystem()
{
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
	for (int i = 0; i < indices.size(); i++)
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
	vertexdata.pSysMem = &vertices_;
	vertexdata.SysMemPitch = 0;
	vertexdata.SysMemSlicePitch = 0;

	//���_�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, &vertexbuffer_);
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
	indexdata.pSysMem = &indices;
	indexdata.SysMemPitch = 0;
	indexdata.SysMemSlicePitch = 0;

	//�C���f�b�N�X�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, &indexbuffer_);
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

	return true;
}

void ParticleSystem::renderBuffer()
{
}

void ParticleSystem::destroyBuffer()
{
}

void ParticleSystem::emitParticle(const float FrameTime)
{
	bool emit;
	bool found;
	Vector3 position;
	float velocity;
	Vector3 color;
	int index;
	std::random_device seed;
	std::mt19937 engine(seed());
	std::normal_distribution<> dist(1.0, 0.5);
	std::unique_ptr<ParticleType> newparticle;

	//�t���[�����Ԃ����Z
	accumulatedtime_ += FrameTime;

	//�t���O���I�t�ɐݒ�
	emit = false;

	//�p�[�e�B�N������o����^�C�~���O���𔻒f
	if (accumulatedtime_ > (1000.0F / data_.perSecond))
	{
		accumulatedtime_ = 0.0F;
		emit = true;
	}

	//���o����p�[�e�B�N��������ꍇ�t���[�����Ƃ�1���o
	if (emit && (accumulatedtime_ < (data_.max - 1)))
	{
		//���������Z
		currentcnt_++;

		//�����_���Ƀv���p�e�B��ݒ肵����
		newparticle.get()->position.x = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.deviation.x;
		newparticle.get()->position.y = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.deviation.y;
		newparticle.get()->position.z = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.deviation.z;

		//���x��ݒ�
		newparticle.get()->velocity = data_.velocity + (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX)* data_.velocityVariation;

		//�F��ݒ�
		newparticle.get()->color.x = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;
		newparticle.get()->color.y = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;
		newparticle.get()->color.z = (static_cast<float>(dist(engine)) - static_cast<float>(dist(engine)) / RAND_MAX) + 0.5F;

		//�ݒ肵���f�[�^�𖖔��ɒǉ�
		particlelist_.push_back(std::move(newparticle));

		//Z�������list���\�[�g
		std::sort(particlelist_.begin()->get(), particlelist_.end()->get());
	}
}

void ParticleSystem::updateParticle(const float FrameTime)
{
	std::list<std::unique_ptr<ParticleType>>::iterator itr;

	//���ׂẴp�[�e�B�N�����X�V
	for (itr = particlelist_.begin(); itr != particlelist_.end(); itr++)
	{
		itr->get()->position.y -= itr->get()->velocity * FrameTime * 0.001F;
	}
}

void ParticleSystem::killParticle()
{
	std::list<std::unique_ptr<ParticleType>>::iterator itr;

	for (itr = particlelist_.begin(); itr != particlelist_.end(); itr++)
	{
		//���̍��W�ɍs������v�f���폜
		if (itr->get()->active && itr->get()->position.y < -3.0F)
		{
			particlelist_.erase(itr);
		}
	}

}
