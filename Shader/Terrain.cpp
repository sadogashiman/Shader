#include "stdafx.h"
#include "Terrain.h"
#include"Support.h"
#include"Direct3D.h"


Terrain::Terrain()
{
	position_ = Vector3::Zero;
	rotation_ = Vector3::Zero;
	modelscale_ = 1.0F;
}

Terrain::~Terrain()
{
}

bool Terrain::init(const wchar_t* ModelFileName, const wchar_t* TextureFileName)
{
	bool result;

	//�e���C�����̎擾
	result = loadTerrainDataFile(ModelFileName);
	if (!result)
	{
		Error::showDialog("�e���C�����̎擾�Ɏ��s");
		return false;
	}

	//�r�b�g�}�b�v�̓ǂݍ���
	result = loadBitmapHeightMap();
	if (!result)
	{
		Error::showDialog("�r�b�g�}�b�v�t�@�C���̓ǂݍ��݂Ɏ��s");
		return false;
	}

	//�ǂݍ��񂾐ݒ���e���C���ɔ��f
	setTerrainCoordinate();
	result = calcNormal();
	if (!result)
	{
		Error::showDialog("�@���̌v�Z�Ɏ��s");
		return false;
	}

	//�n�`���𐶐�
	result = buildTerrainModel();
	if (!result)
	{
		Error::showDialog("�n�`���̐����Ɏ��s");
		return false;
	}

	//�n�`���𐶐������̂�heightmap���폜
	destroyHightMap();

	//�o�b�t�@�̏�����
	result = initbuffer();
	if (!result)
	{
		Error::showDialog("�o�b�t�@�̏������Ɏ��s");
		return false;
	}

	//�o�b�t�@���쐬���ꂽ�̂Ń��f������j��
	destroyTerrainModel();

	if (TextureFileName != nullptr)
	{
		//�t�@�C�����������o�ɕێ�
		wcscpy(texturefilename_, TextureFileName);
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
	instanceptr_->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);

	//�C���f�b�N�X�o�b�t�@���Z�b�g
	instanceptr_->getContext()->IASetIndexBuffer(indexbuffer_.Get(), DXGI_FORMAT_R32_UINT, 0);

	//�v���~�e�B�u�̃��[�h��ݒ�
	instanceptr_->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Terrain::destroy()
{
	destroyHightMap();
	destroyTerrainModel();
}

Matrix Terrain::getWorldMatrix()
{
	world_ = Matrix::Identity;
	world_ *= Matrix::CreateScale(modelscale_);
	world_ *= Matrix::CreateRotationX(XMConvertToRadians(rotation_.x));
	world_ *= Matrix::CreateRotationY(XMConvertToRadians(rotation_.y));
	world_ *= Matrix::CreateRotationZ(XMConvertToRadians(rotation_.z));
	world_ *= Matrix::CreateTranslation(position_);

	return world_;
}

bool Terrain::initbuffer()
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
	Vector4 color;
	Vector3 position;

	//�e���C���O���b�h�̃T�C�Y��ݒ�
	terrainwidth = 256;
	terrainheight = 256;

	//�O���b�h�̐F��ݒ�
	color = Vector4::One;

	//���_�����v�Z
	vertexcnt_ = (terrainwidth - 1) * (terrainheight - 1) * 6;

	//�C���f�b�N�X����ݒ�
	indexcnt_ = vertexcnt_;

	//���_�z����쐬
	vertices.resize(vertexcnt_);

	//�C���f�b�N�X�z����쐬
	indices.resize(indexcnt_);

	//���_�z��Ƀf�[�^�����[�h
	for (int i = 0; i < vertexcnt_; i++)
	{
		vertices[i].position = model_[i].position;
		vertices[i].texture = model_[i].texture;
		vertices[i].normal = model_[i].normal;
		indices[i] = i;
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
	hr = instanceptr_->getDevice()->CreateBuffer(&vertexbufferdesc, &vertexdata, vertexbuffer_.GetAddressOf());
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
	hr = instanceptr_->getDevice()->CreateBuffer(&indexbufferdesc, &indexdata, indexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	return true;
}

bool Terrain::loadTerrainDataFile(const wchar_t* FileName)
{
	std::ifstream fin;
	char input;

	//�f�[�^�t�@�C���W�J
	if (!Support::searchFile(FileName))
	{
		Error::showDialog("�f�[�^�t�@�C���ւ̃t�@�C���p�X�������ł�");
		return false;
	}

	fin.open(FileName);
	if (fin.fail())
	{
		Error::showDialog("�f�[�^�t�@�C���̓W�J�Ɏ��s");
		return false;
	}

	//�e���C���t�@�C�����܂Ŕ�΂�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//�t�@�C������ǂݎ��
	fin >> terrainfilename_;

	//�������܂Ŕ�΂�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//������ǂݎ��
	fin >> terrainheight_;

	//�����܂Ŕ�΂�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//������ǂݎ��
	fin >> terrainwidth_;

	//�X�P�[���l�܂Ŕ�΂�
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	//�X�P�[�����擾
	fin >> heightscale_;

	//�t�@�C�������
	fin.close();

	return true;
}

bool Terrain::loadBitmapHeightMap()
{
	int imagesize;
	int index;
	int error;
	FILE* fp;
	unsigned long long cnt;
	BITMAPFILEHEADER bitmapfileheader;
	BITMAPINFOHEADER bitmapinfoheader;
	std::vector<unsigned char> bitmapimage;
	unsigned char height;

	//���i�[�z����쐬
	heightmap_.resize(terrainheight_ * terrainwidth_);

	//�r�b�g�}�b�v�t�@�C�����o�C�i���œW�J
	if (!Support::searchFile(terrainfilename_))
	{
		Error::showDialog("�r�b�g�}�b�v�t�@�C���p�X�������ł�");
		return false;
	}

	//�r�b�g�}�b�v�W�J
	error = fopen_s(&fp, terrainfilename_, "rb");
	if (error != 0)
	{
		Error::showDialog("�t�@�C���W�J�Ɏ��s");
		return false;
	}

	//�r�b�g�}�b�v�̃w�b�_�[������ǂݎ��
	cnt = fread(&bitmapfileheader, sizeof(BITMAPFILEHEADER), 1, fp);
	if (cnt != 1)
	{
		Error::showDialog("�w�b�_�[�����̓ǂݎ��Ɏ��s");
		return false;
	}

	//�r�b�g�}�b�v����ǂݎ��
	cnt = fread(&bitmapinfoheader, sizeof(BITMAPINFOHEADER), 1, fp);
	if (cnt != 1)
	{
		Error::showDialog("�r�b�g�}�b�v���w�b�_�[�̓ǂݎ��Ɏ��s");
		return false;
	}

	//height�}�b�v�̎������e���C���Ɠ������m�F
	if ((bitmapinfoheader.biHeight != terrainheight_) || (bitmapinfoheader.biWidth != terrainwidth_))
	{
		Error::showDialog("Height�}�b�v�̎����ƃv���O�������̒n�`�̎�������v���܂���");
		return false;
	}

	//�摜�f�[�^�T�C�Y���擾
	imagesize = terrainheight_ * ((terrainwidth_ * 3) + 1);

	//�摜�f�[�^�Ƀ����������蓖��
	bitmapimage.resize(imagesize);

	//�t�@�C���|�C���^��擪�ֈړ�
	fseek(fp, bitmapfileheader.bfOffBits, SEEK_SET);

	//�摜�f�[�^��ǂݎ��
	cnt = fread(&bitmapimage[0], 1, imagesize, fp);
	if (cnt != imagesize)
	{
		Error::showDialog("�摜�f�[�^�̓ǂݍ��݂Ɏ��s");
		return false;
	}

	//�t�@�C�������
	error = fclose(fp);
	if (error != 0)
	{
		Error::showDialog("�t�@�C���N���[�Y�Ɏ��s");
		return false;
	}

	//�o�b�t�@�̈ʒu����
	int k = 0;

	for (int i = 0; i < terrainheight_; i++)
	{
		for (int j = 0; j < terrainwidth_; j++)
		{
			//�r�b�g�}�b�v�͏㉺���t�Ȃ̂ŉ������փ��[�h
			index = (terrainwidth_ * (terrainheight_ - 1 - i)) + j;

			//�摜����O���[�X�P�[���̃s�N�Z���l���擾
			height = bitmapimage[k];

			//�������Ƃ��ăs�N�Z���l���i�[
			heightmap_[index].position.y = static_cast<float>(height);

			//�r�b�g�}�b�v�f�[�^�C���f�b�N�X���C���N�������g
			k += 3;
		}

		//�r�b�g�}�b�v��␳
		k++;
	}

	//�r�b�g�}�b�v�摜�f�[�^�����
	bitmapimage.clear();

	return true;
}

void Terrain::setTerrainCoordinate()
{
	int index;

	//���W�𒲐�
	for (int i = 0; i < terrainheight_; i++)
	{
		for (int j = 0; j < terrainwidth_; j++)
		{
			index = (terrainwidth_ * i) + j;

			//X���W�EZ���W��ݒ�
			heightmap_[index].position.x = static_cast<float>(j);
			heightmap_[index].position.z = -static_cast<float>(i);

			//�k�x���𐳂̒l�����ɂȂ�悤�ړ�
			heightmap_[index].position.z += static_cast<float>(terrainheight_ - 1);

			//�������X�P�[�����O
			heightmap_[index].position.y /= heightscale_;
		}
	}

}

bool Terrain::buildTerrainModel()
{
	int index;
	int index1;
	int index2;
	int index3;
	int index4;

	//���_�����v�Z
	vertexcnt_ = (terrainheight_ - 1) * (terrainwidth_ - 1) * 6;

	//���_�z����쐬
	model_.resize(vertexcnt_);

	index = 0;

	int width = terrainwidth_ - 1;
	int height = terrainheight_ - 1;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			index1 = (terrainwidth_ * i) + j;				//����
			index2 = (terrainwidth_ * i) + (j + 1);			//�E��
			index3 = (terrainwidth_ * (i + 1)) + j;			//����
			index4 = (terrainwidth_ * (i + 1)) + (j + 1);	//�E��

			//�O�p�`�𐶐�
			model_[index].position = heightmap_[index1].position;
			model_[index].texture.x = 0.0F;
			model_[index].texture.y = 0.0F;
			model_[index].normal = heightmap_[index1].normal;
			index++;

			model_[index].position = heightmap_[index2].position;
			model_[index].texture.x = 1.0F;
			model_[index].texture.y = 0.0F;
			model_[index].normal = heightmap_[index2].normal;
			index++;

			model_[index].position = heightmap_[index3].position;
			model_[index].texture.x = 0.0F;
			model_[index].texture.y = 1.0F;
			model_[index].normal = heightmap_[index3].normal;
			index++;

			model_[index].position = heightmap_[index3].position;
			model_[index].texture.x = 0.0F;
			model_[index].texture.y = 1.0F;
			model_[index].normal = heightmap_[index3].normal;
			index++;

			model_[index].position = heightmap_[index2].position;
			model_[index].texture.x = 1.0F;
			model_[index].texture.y = 0.0F;
			model_[index].normal = heightmap_[index2].normal;
			index++;

			model_[index].position = heightmap_[index4].position;
			model_[index].texture.x = 1.0F;
			model_[index].texture.y = 1.0F;
			model_[index].normal = heightmap_[index4].normal;
			index++;
		}
	}

	return true;
}

bool Terrain::calcNormal()
{
	int index;
	int index1;
	int index2;
	int index3;
	float vertex[3][3];
	float vector[2][3];
	float sum[3];
	float length;
	std::vector<VectorType> normal;

	//�z��T�C�Y��ύX
	normal.resize((terrainheight_ - 1) * (terrainwidth_ - 1));

	for (int i = 0; i < terrainheight_-1; i++)
	{
		for (int j = 0; j < terrainwidth_-1; j++)
		{

			//�O�p�`���`��
			index1 = ((i + 1) * terrainwidth_) + j;
			index2 = ((i + 1) * terrainwidth_) + (j + 1);
			index3 = (i * terrainwidth_) + j;

			//�t�F�[�X���璸�_���擾
			vertex[0][0] = heightmap_[index1].position.x;
			vertex[0][1] = heightmap_[index1].position.y;
			vertex[0][2] = heightmap_[index1].position.z;

			vertex[1][0] = heightmap_[index2].position.x;
			vertex[1][1] = heightmap_[index2].position.y;
			vertex[1][2] = heightmap_[index2].position.z;

			vertex[2][0] = heightmap_[index3].position.x;
			vertex[2][1] = heightmap_[index3].position.y;
			vertex[2][2] = heightmap_[index3].position.z;

			//�x�N�g�����v�Z
			vector[0][0] = vertex[0][0] - vertex[2][0];
			vector[0][1] = vertex[0][1] - vertex[2][1];
			vector[0][2] = vertex[0][2] - vertex[2][2];
			vector[1][0] = vertex[2][0] - vertex[1][0];
			vector[1][1] = vertex[2][1] - vertex[1][1];
			vector[1][2] = vertex[2][2] - vertex[1][2];

			index = (i * (terrainwidth_ - 1)) + j;

			//�O�ς��v�Z
			normal[index].position.x = (vector[0][1] * vector[1][2]) - (vector[0][2] * vector[1][1]);
			normal[index].position.y = (vector[0][2] * vector[1][0]) - (vector[0][0] * vector[1][2]);
			normal[index].position.z = (vector[0][0] * vector[1][1]) - (vector[0][1] * vector[1][0]);

			//�������v�Z
			length = static_cast<float>(std::sqrt((normal[index].position.x * normal[index].position.x) + (normal[index].position.y * normal[index].position.y) + (normal[index].position.z * normal[index].position.z)));

			//���������Ƃɐ��K��
			normal[index].position.x = (normal[index].position.x / length);
			normal[index].position.y = (normal[index].position.y / length);
			normal[index].position.z = (normal[index].position.z / length);
		}
	}

	//���ׂĂ̒��_���炱�̒��_�ɂɐڂ���ʖ@���̍��v�����
	for (int i = 0; i < terrainheight_; i++)
	{
		for (int j = 0; j < terrainwidth_; j++)
		{
			//���v�l��������
			sum[0] = 0.0F;
			sum[1] = 0.0F;
			sum[2] = 0.0F;

			//�����̃t�F�[�X
			if (((j - 1) >= 0) && ((i - 1) >= 0))
			{
				index = ((i - 1) * (terrainwidth_ - 1)) + (j - 1);

				sum[0] += normal[index].position.x;
				sum[1] += normal[index].position.y;
				sum[2] += normal[index].position.z;
			}

			//�E���̃t�F�[�X
			if ((j < (terrainwidth_ - 1)) && ((i - 1) >= 0))
			{
				index = ((i - 1) * (terrainwidth_ - 1)) + j;

				sum[0] += normal[index].position.x;
				sum[1] += normal[index].position.y;
				sum[2] += normal[index].position.z;
			}

			//����̃t�F�[�X
			if (((j - 1) >= 0) && (i < (terrainheight_ - 1)))
			{
				index = (i * (terrainheight_ - 1)) + (j - 1);

				sum[0] += normal[index].position.x;
				sum[1] += normal[index].position.y;
				sum[2] += normal[index].position.z;
			}

			//�E��̃t�F�[�X
			if ((j < (terrainwidth_ - 1)) && (i < terrainheight_ - 1))
			{
				index = (i * (terrainwidth_ - 1)) + j;

				sum[0] += normal[index].position.x;
				sum[1] += normal[index].position.y;
				sum[2] += normal[index].position.z;
			}

			//�@���̒������v�Z
			length = std::sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			//�C���f�b�N�X���v�Z
			index = (i * terrainwidth_) + j;

			//���K��
			heightmap_[index].normal.x = (sum[0] / length);
			heightmap_[index].normal.y = (sum[1] / length);
			heightmap_[index].normal.z = (sum[2] / length);
		}
	}

	//�z������
	normal.clear();

	return true;
}

void Terrain::destroyHightMap()
{
	//hright�}�b�v�����
	if (heightmap_.size() > 0)
	{
		heightmap_.clear();
	}
}

void Terrain::destroyTerrainModel()
{
	//�e���C�����f�����
	if (model_.size() > 0)
	{
		model_.clear();
	}
}
