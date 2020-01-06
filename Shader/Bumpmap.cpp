#include "stdafx.h"
#include "Bumpmap.h"
#include"Direct3D.h"
#include"checkinputlayout.h"

Bumpmap::Bumpmap()
{
	layout_ = nullptr;
	lightbuffer_ = nullptr;
	matrixbuff_ = nullptr;
	pixelshader_ = nullptr;
	vertexshader_ = nullptr;
	samplerstate_ = nullptr;
}

Bumpmap::~Bumpmap()
{
}

bool Bumpmap::init()
{
	if (!initShader( L"bumpmapvs.hlsl", L"bumpmapps.hlsl"))
	{
		return false;
	}

	return true;
}

void Bumpmap::destroy()
{
	destroyShader();
}

bool Bumpmap::render(const int Indexcount, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView ** TextureArray, Vector3 LightDirection, Vector4 DiffuseColor)
{
	if (!SetShaderParameters(World, View, Projection, TextureArray, LightDirection, DiffuseColor))
	{
		return false;
	}

	renderShader(Indexcount);

	return true;
}

bool Bumpmap::initShader( std::filesystem::path vsFileName, std::filesystem::path psFileName)
{
	HRESULT hr;
	ID3D10Blob* vertexshaderbuffer;
	ID3D10Blob* pixelshaderbuffer;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[5];
	unsigned int numelements;
	D3D11_SAMPLER_DESC samplerdesc;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc;

	vertexshaderbuffer = nullptr;
	pixelshaderbuffer = nullptr;

	//�V�F�[�_�[�R���p�C��
	hr = D3DCompileFromFile(psFileName.c_str(), NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr = D3DCompileFromFile(vsFileName.c_str(), NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	//�V�F�[�_�[�쐬
	hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer->GetBufferPointer(), pixelshaderbuffer->GetBufferSize(), nullptr, &pixelshader_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), NULL, &vertexshader_);
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

	polygonlayout[3].SemanticName = "TANGENT";
	polygonlayout[3].SemanticIndex = 0;
	polygonlayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[3].InputSlot = 0;
	polygonlayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[3].InstanceDataStepRate = 0;

	polygonlayout[4].SemanticName = "BINORMAL";
	polygonlayout[4].SemanticIndex = 0;
	polygonlayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[4].InputSlot = 0;
	polygonlayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[4].InstanceDataStepRate = 0;

	//���C�A�E�g���̗v�f�̐����擾
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	if (!checkInputLayout(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
	//���_���̓��C�A�E�g�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), &layout_);
	if (FAILED(hr))
	{
		return false;
	}

	//�s�v�ɂȂ����̂ō폜
	vertexshaderbuffer->Release();
	vertexshaderbuffer = nullptr;
	pixelshaderbuffer->Release();
	pixelshaderbuffer = nullptr;

	//���I�}�g���b�N�X�萔�o�b�t�@�̐ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//���̃N���X���璸�_�V�F�[�_�̒萔�o�b�t�@�ɃA�N�Z�X�ł���悤�Ƀ|�C���^���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuff_);
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
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplerstate_);
	if (FAILED(hr))
	{
		return false;
	}

	//�s�N�Z���V�F�[�_�[�ɂ��郉�C�g�_�C�i�~�b�N�萔�o�b�t�@�̐ݒ�
	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	//���̃N���X�����璸�_�V�F�[�_�[�̒萔�o�b�t�@�ɃA�N�Z�X�ł���悤�Ƀ|�C���^���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, &lightbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Bumpmap::destroyShader()
{
	SAFE_RELEASE(lightbuffer_);
	SAFE_RELEASE(matrixbuff_);
	SAFE_RELEASE(samplerstate_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool Bumpmap::SetShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView ** TextureArray, Vector3 LightDirection, Vector4 DiffuseColor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	unsigned int buffnumber;

	//�}�g���b�N�X��]�u���ăV�F�[�_�[�p�ɏ���
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�������݉\�Ȃ悤�ɒ萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuff_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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
	Direct3D::getInstance()->getContext()->Unmap(matrixbuff_, 0);

	//���_�V�F�[�_�Œ萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Œ��_�V�F�[�_�̒萔�o�b�t�@���Ō�ɐݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuff_);

	//�s�N�Z���V�F�[�_�[�Ńe�N�X�`�����\�[�X��ݒ�
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 2, TextureArray);

	//���C�g�萔�o�b�t�@�����b�N���ď������݉\��
	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//�萔�o�b�t�@���̃f�[�^�ւ̃|�C���^���擾
	dataptr2 = (LightBufferType*)mappedresouce.pData;

	//�Ɩ��ϐ���萔�o�b�t�@�ɃR�s�[
	dataptr2->diffuseColor = DiffuseColor;
	dataptr2->lightDirection = LightDirection;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_, 0);

	//�s�N�Z���V�F�[�_�[�Ń��C�g�萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Ńs�N�Z���V�F�[�_�[�̃��C�g�萔�o�b�t�@��ݒ�
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, &lightbuffer_);
	return true;
}

void Bumpmap::renderShader( const int Indexcount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);
	//���̎O�p�`�̃����_�����O�Ɏg�p����钸�_�V�F�[�_�ƃV�F�[�_��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//�T���v���[��Ԃ��s�N�Z���V�F�[�_�[�ɐݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplerstate_);

	//�O�p�`�������_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
