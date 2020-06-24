#include "stdafx.h"
#include "TerrainShader.h"
#include "Direct3D.h"

bool TerrainShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[3];
	unsigned int numelements;
	D3D11_SAMPLER_DESC sampledesc;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc;

	//�T�|�[�g�N���X�𐶐�
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̐����Ɏ��s");
		return false;
	}

	//�V�F�[�_�[�쐬
	hr = support_.get()->createVertexData(L"terrain_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"terrain_ps.cso", pixelshader_.GetAddressOf());
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

	//�v�f�����Z�o
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	//���_���̓��C�A�E�g�̍쐬
	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements, layout_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�T���v���[�̐ݒ�
	sampledesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampledesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.MipLODBias = 0.0F;
	sampledesc.MaxAnisotropy = 1;
	sampledesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampledesc.BorderColor[0] = 0;
	sampledesc.BorderColor[1] = 0;
	sampledesc.BorderColor[2] = 0;
	sampledesc.BorderColor[3] = 0;
	sampledesc.MinLOD = 0;
	sampledesc.MaxLOD = D3D11_FLOAT32_MAX;

	//�T���v���[�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&sampledesc, samplestate_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�T���v���[�̍쐬�Ɏ��s");
		return false;
	}

	//���_�V�F�[�_�[�p�萔�o�b�t�@�̐ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//���_�V�F�[�_�[�p�萔�o�b�t�@���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�p�萔�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�s�N�Z���V�F�[�_�[�p�萔�o�b�t�@�̐ݒ�
	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	//�s�N�Z���V�F�[�_�[�p�萔�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, lightbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�p�萔�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	return true;
}

bool TerrainShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, Vector4 AmbientColor, Vector4 DiffuseColor, Vector3 LightDirection, ID3D11ShaderResourceView* Texture)
{
	bool result;

	result = setShaderParameters(World, View, Projection, AmbientColor, DiffuseColor, LightDirection,Texture);
	if (!result)
	{
		Error::showDialog("�V�F�[�_�[�p�����[�^�[�̐ݒ�Ɏ��s");
		return false;
	}

	renderShader(IndexCount);

	return true;
}

bool TerrainShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 AmbientColor, Vector4 DiffuseColor, Vector3 LightDirection, ID3D11ShaderResourceView* Texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	unsigned int buffernumber;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;

	//�V�F�[�_�[�p�ɍs���]�u
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�|�C���^���L���X�g
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//�f�[�^�X�V
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//���b�N����
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�萔�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N");
		return false;
	}

	//�|�C���^���L���X�g
	dataptr2 = (LightBufferType*)mappedresource.pData;

	//�|�C���^�L���X�g
	dataptr2->ambientcolor = AmbientColor;
	dataptr2->diffusecolor = DiffuseColor;
	dataptr2->lightdirection = LightDirection;
	dataptr2->padding = 0.0F;

	//���b�N����
	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�萔�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, lightbuffer_.GetAddressOf());

	//�e�N�X�`���Z�b�g
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);
	return true;
}

void TerrainShader::renderShader(const int IndexCount)
{
	//���_���̓��C�A�E�g�̃Z�b�g
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�V�F�[�_�[�Z�b�g
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	
	//�T���v���[�Z�b�g
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplestate_.GetAddressOf());

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);
}