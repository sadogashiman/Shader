#include "stdafx.h"
#include "Colorshader.h"
#include "Direct3D.h"


Colorshader::Colorshader()
{
}

Colorshader::~Colorshader()
{
}

bool Colorshader::init()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	hr = support_.get()->createVertexData(L"colorvs.hlsl", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support_.get()->createPixelData(L"colorps.hlsl", pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//���_���̓��C�A�E�g�̍쐬
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	polygonlayout[1].SemanticName = "COLOR";
	polygonlayout[1].SemanticIndex = 0;
	polygonlayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonlayout[1].InputSlot = 0;
	polygonlayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[1].InstanceDataStepRate = 0;

	//�v�f�����Z�o
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	//���_���̓��C�A�E�g�̍쐬
	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements, layout_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_���̓��C�A�E�g�̍쐬�Ɏ��s");
		return false;
	}

	//�萔�o�b�t�@�̐ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//�萔�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�萔�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}


	return true;
}

bool Colorshader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection)
{
	bool result;

	//�V�F�[�_�[�p�����[�^�[�̐ݒ�
	result = setShaderParameters(World, View, Projection);
	if (!result)
	{
		Error::showDialog("�V�F�[�_�[�p�����[�^�[�̐ݒ�Ɏ��s");
		return false;
	}

	//�����_�����O
	renderShader(IndexCount);

	return true;
}

bool Colorshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	unsigned int buffernumber;

	//�V�F�[�_�[�悤�ɍs���]�u
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�o�b�t�@�̃��b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�萔�o�b�t�@�ւ̃|�C���^���쐬
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

	return true;
}

void Colorshader::renderShader(const int IndexCount)
{
	//���_���̓��C�A�E�g�̍쐬
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�V�F�[�_�[�Z�b�g
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount,0,0);
}