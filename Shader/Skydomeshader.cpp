#include "stdafx.h"
#include "Skydomeshader.h"
#include "Direct3D.h"

Skydomeshader::Skydomeshader()
{
	ZeroMemory(this, sizeof(Skydomeshader));
}

Skydomeshader::~Skydomeshader()
{

}

bool Skydomeshader::init()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC gradientbufferdesc;
	unsigned int numelements;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	//�V�F�[�_�[��ǂݍ���
	hr = support_.get()->createVertexData(L"skydome_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"skydome_ps.cso", pixelshader_.GetAddressOf());
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

	//�v�f�����Z�o
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	//���_���̓��C�A�E�g�̍쐬
	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements, layout_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_���̓��C�A�E�g�̍쐬�Ɏ��s");
		return false;
	}

	//�}�g���b�N�X�o�b�t�@�̐ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//�}�g���b�N�X�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�}�g���b�N�X�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�O���f�[�V�����p�o�b�t�@�̐ݒ�
	gradientbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientbufferdesc.ByteWidth = sizeof(GradientBufferType);
	gradientbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientbufferdesc.MiscFlags = 0;
	gradientbufferdesc.StructureByteStride = 0;

	//�O���f�[�V�����p�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&gradientbufferdesc, NULL, gradientbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�O���f�[�V�����p�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	return true;
}

bool Skydomeshader::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	bool result;

	result = setShaderParameters(World, View, Projection, ApexColor, CentorColor);
	if (!result)
	{
		Error::showDialog("�V�F�[�_�[�p�����[�^�[�̐ݒ�Ɏ��s");
		return false;
	}

	//�����_�����O
	renderShader(Indexcount);

	return true;
}

bool Skydomeshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	GradientBufferType* dataptr2;
	unsigned int buffernumber;

	//�s����V�F�[�_�[�p�ɓ]�u
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�o�b�t�@���b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�V�F�[�_�[���̃f�[�^�ւ̃|�C���^���쐬
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//�f�[�^���X�V
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//���b�N����
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�萔�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//�o�b�t�@���b�N
	hr = Direct3D::getInstance()->getContext()->Map(gradientbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�V�F�[�_�[���̃f�[�^�ւ̃|�C���^���쐬
	dataptr2 = (GradientBufferType*)mappedresource.pData;

	//�f�[�^���X�V
	dataptr2->apexcolor = ApexColor;
	dataptr2->centorcolor = CentorColor;

	//���b�N����
	Direct3D::getInstance()->getContext()->Unmap(gradientbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�X�V�����l�Ńs�N�Z���V�F�[�_�[�̒萔�o�b�t�@���X�V
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, gradientbuffer_.GetAddressOf());

	return true;
}

void Skydomeshader::renderShader(const int IndexCount)
{
	//���_���̓��C�A�E�g���Z�b�g
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�V�F�[�_�[���Z�b�g
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(),NULL, 0);
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);
}