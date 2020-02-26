#include "stdafx.h"
#include "Maskshader.h"
#include"Support.h"
#include"Direct3D.h"

Maskshader::Maskshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	matrixbuffer_ = nullptr;
	samplestate_ = nullptr;
}

Maskshader::~Maskshader()
{
}

bool Maskshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;
	Support support;

	//�V�F�[�_�[�ǂݍ���
	hr = support.createVertexData(L"maskvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support.createPixelData(L"maskps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//�쐬���ꂽ�f�[�^���R�s�[
	vertexshader_ = support.getVertexShader();
	pixelshader_ = support.getPixelShader();

	//���_���̓��C�A�E�g�̐ݒ�
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	polygonlayout[1].SemanticName = "TEXCOORD";
	polygonlayout[1].SemanticIndex = 0;
	polygonlayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonlayout[1].InputSlot = 0;
	polygonlayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[1].InstanceDataStepRate = 0;

	//���C�A�E�g���̗v�f�̐����擾
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

#ifdef _DEBUG
	//�f�[�^���L�����m�F
	if (!Support::checkInputLayout(support.getVertexBufferPtr(), support.getVertexBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif // _DEBUG
	//���_���̓��C�A�E�g�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, support.getVertexBufferPtr(), support.getVertexBufferSize(), &layout_);
	if (FAILED(hr))
	{
		return false;
	}

	//���I�}�g���b�N�X�萔�o�b�t�@�̐ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//���̃N���X���璸�_�V�F�[�_�̒萔�o�b�t�@�ɃA�N�Z�X�ł���悤�Ƀ|�C���^���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	//�T���v���[�̐ݒ�
	samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.MipLODBias = 0.0F;
	samplerdesc.MaxAnisotropy = 1;
	samplerdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerdesc.BorderColor[0] = 0;
	samplerdesc.BorderColor[1] = 0;
	samplerdesc.BorderColor[2] = 0;
	samplerdesc.BorderColor[3] = 0;
	samplerdesc.MinLOD = 0;
	samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;

	//�e�N�X�`���̃T���v���[��Ԃ�ݒ�
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplestate_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Maskshader::destroy()
{
	destroyshader();
}

bool Maskshader::render(int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	bool result;

	result = setShaderParameters(World, View, Projection, TextureArray);
	if (!result)
	{
		return false;
	}

	//�V�F�[�_�[�����_�����O
	rendershader(Indexcount);

	return true;
}

void Maskshader::destroyshader()
{
	//�j��
	if (samplestate_)
	{
		samplestate_->Release();
		samplestate_ = nullptr;
	}

	if (matrixbuffer_)
	{
		matrixbuffer_->Release();
		matrixbuffer_ = nullptr;
	}

	if (layout_)
	{
		layout_->Release();
		layout_ = nullptr;
	}

	if (pixelshader_)
	{
		pixelshader_->Release();
		pixelshader_ = nullptr;
	}

	if (vertexshader_)
	{
		vertexshader_->Release();
		vertexshader_ = nullptr;
	}
}

bool Maskshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	unsigned int buffnumber;

	//�}�g���b�N�X��]�u���ăV�F�[�_�[�p�ɏ���
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�������݉\�Ȃ悤�ɒ萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//�萔�o�b�t�@���̃f�[�^�ւ̃|�C���^���擾
	dataptr = (MatrixBufferType*)mappedresouce.pData;

	//�s���萔�o�b�t�@�ɃR�s�[
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//���_�V�F�[�_�Œ萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Œ��_�V�F�[�_�̒萔�o�b�t�@���Ō�ɐݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuffer_);

	//�s�N�Z���V�F�[�_�[�ŃV�F�[�_�[���\�[�X�e�N�X�`���z���ݒ�
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 3, TextureArray);

	return true;
}

void Maskshader::rendershader(int Indexcount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//���̎O�p�`�̃����_�����O�Ɏg�p����钸�_�V�F�[�_�ƃs�N�Z���V�F�[�_��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//�T���v���[��Ԃ��s�N�Z���V�F�[�_�[�ɐݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplestate_);

	//�O�p�`�������_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
