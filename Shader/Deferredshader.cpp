#include "stdafx.h"
#include "Deferredshader.h"
#include"Direct3D.h"


Deferredshader::Deferredshader()
{
	ZeroMemory(this, sizeof(Deferredshader));
}

Deferredshader::~Deferredshader()
{
}

bool Deferredshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[3];
	D3D11_SAMPLER_DESC samplerdesc;
	D3D11_BUFFER_DESC matrixbufferdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	//�V�F�[�_�[�ǂݍ���
	hr = support_.get()->createVertexData(L"Shader/deferred_vs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/deferred_ps.cso",pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

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

	polygonlayout[2].SemanticName = "NORMAL";
	polygonlayout[2].SemanticIndex = 0;
	polygonlayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[2].InputSlot = 0;
	polygonlayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[2].InstanceDataStepRate = 0;


	//���_���̓��C�A�E�g���쐬
	hr = support_.get()->createVertexInputLayout(polygonlayout, _countof(polygonlayout), layout_.GetAddressOf());
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

	//�e�N�X�`���T���v���[���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplestatewrap_.GetAddressOf());
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

	//���_�V�F�[�_�[�̒萔�o�b�t�@�ɃA�N�Z�X�ł���悤�Ƀ|�C���^���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Deferredshader::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	bool result;

	//�����_�����O�Ɏg�p����V�F�[�_�[�p�����[�^�[��ݒ�
	result = setShaderParameters(World, View, Projection, Texture);
	if (!result)
	{
		return false;
	}

	//�������ꂽ�o�b�t�@�������_�����O
	renderShader(Indexcount);

	return true;
}

bool Deferredshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	unsigned int buffernumber;
	MatrixBufferType* dataptr;

	//�s���]�u���ăV�F�[�_�[�悤�ɏ���
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		return false;
	}

	//�萔�o�b�t�@���̃f�[�^�ւ̃|�C���^���擾
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//�s���萔�o�b�t�@�ɃR�s�[
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//���_�V�F�[�_�[�Œ萔�o�b�t�@�̈ʒu��ݒ�
	buffernumber = 0;

	//�X�V���ꂽ�l���g�p���Ē��_�V�F�[�_�[�̒萔�o�b�t�@��ݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//�s�N�Z���V�F�[�_�[�ŃV�F�[�_�[�e�N�X�`�����\�[�X��ݒ�
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);

	return true;
}

void Deferredshader::renderShader(const int Indexcount)
{
	//���_���̓��C�A�E�g�̐ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�����_�����O�Ɏg�p����钸�_�V�F�[�_�[�ƃs�N�Z���V�F�[�_�[��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//�T���v���[�̏�Ԃ��s�N�Z���V�F�[�_�[�ɐݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplestatewrap_.GetAddressOf());

	//�W�I���g���������_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
