#include "stdafx.h"
#include "Shadowshader.h"
#include"Direct3D.h"


Shadowshader::Shadowshader()
{
	ZeroMemory(this, sizeof(Shadowshader));
}

Shadowshader::~Shadowshader()
{
}

bool Shadowshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[3];
	unsigned int numelements;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	//�V�F�[�_�[�ǂݍ���
	hr = support_.get()->createVertexData(L"shadow_vs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"shadow_ps.cso",pixelshader_.GetAddressOf());
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

	//���C�A�E�g���̗v�f�̐����擾
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	//���_���̓��C�A�E�g���쐬
	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements,layout_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�T���v���[�̐ݒ�
	samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerdesc.MipLODBias = 0.0F;
	samplerdesc.MaxAnisotropy = 1;
	samplerdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerdesc.BorderColor[0] = 0;
	samplerdesc.BorderColor[1] = 0;
	samplerdesc.BorderColor[2] = 0;
	samplerdesc.BorderColor[3] = 0;
	samplerdesc.MinLOD = 0;
	samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;

	//�e�N�X�`���̃T���v���[���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplestateclamp_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�T���v���^�C�v�̈Ⴄ���̂��쐬
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplestatewrap_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�萔�o�b�t�@��ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//�萔�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//���C�g�o�b�t�@�̐ݒ�
	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	//���C�g�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, lightbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	lightbufferdesc.ByteWidth = sizeof(LightBufferType2);

	//��ڂ̃��C�g�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, lightbuffer2_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Shadowshader::render(const int Indexcound,Matrix World,Matrix View,Matrix Projection,Matrix lightview,Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 LightDirection, Vector4 Ambientcolor, Vector4 Diffusecolor)
{	bool result;

	//�����_�����O�Ɋւ���V�F�[�_�[�p�����[�^��ݒ�
	result = setShaderParameters(World, View, Projection, lightview, lightprojection, texture, Depthmaptexture, LightDirection, Ambientcolor, Diffusecolor);

	if (!result)
	{
		return false;
	}

	//���������o�b�t�@���V�F�[�_�[�Ń����_�����O
	renderShader(Indexcound);

	return true;
}

bool Shadowshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix Lightview, Matrix Lightprojection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 LightDirection, Vector4 Ambientcolor, Vector4 Diffusecolor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	LightBufferType2* dataptr3;
	unsigned int buffnumber;

	//�V�F�[�_�[�悤�ɍs���]�u
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);
	Lightview = XMMatrixTranspose(Lightview);
	Lightprojection = XMMatrixTranspose(Lightprojection);

	//�������݉\�Ȃ悤�ɒ萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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
	dataptr->lightview = Lightview;
	dataptr->lightprojection = Lightprojection;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//���_�V�F�[�_�Œ萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Œ��_�V�F�[�_�̒萔�o�b�t�@���Ō�ɐݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, matrixbuffer_.GetAddressOf());

	//�V�F�[�_�[���\�[�X��ݒ�
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);
	Direct3D::getInstance()->getContext()->PSSetShaderResources(1, 1, &Depthmaptexture);

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD,0, & mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	dataptr2 = (LightBufferType*)mappedresouce.pData;

	//�f�[�^�X�V
	dataptr2->ambientColor = Ambientcolor;
	dataptr2->diffuseColor = Diffusecolor;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_.Get(), 0);

	buffnumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, lightbuffer_.GetAddressOf());

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer2_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//�|�C���^�L���X�g
	dataptr3 = (LightBufferType2*)mappedresouce.pData;

	dataptr3->lightposition = LightDirection;
	dataptr3->padding = 0.0F;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer2_.Get(), 0);

	buffnumber = 1;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, lightbuffer2_.GetAddressOf());

	return true;
}

void Shadowshader::renderShader(const int Indexcount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�����_�����O�Ɏg�p����V�F�[�_�[��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//�T���v���[�̐ݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplestateclamp_.GetAddressOf());
	Direct3D::getInstance()->getContext()->PSSetSamplers(1, 1, samplestatewrap_.GetAddressOf());

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
