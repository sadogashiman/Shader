#include "stdafx.h"
#include "SkyDome.h"
#include"Direct3D.h"

bool SkyDome::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	GradientBufferType* dataptr2;
	unsigned int buffernumber;

	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//���b�N�����o�b�t�@�̃|�C���^���擾
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//�萔�o�b�t�@�ɃR�s�[
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�萔�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//�O���f�[�V�����p�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(gradientbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//���b�N�����o�b�t�@�̃|�C���^���擾
	dataptr2 = (GradientBufferType*)mappedresource.pData;

	//�萔�o�b�t�@�ɃR�s�[
	dataptr2->apexcolor = ApexColor;
	dataptr2->centorcolor = CentorColor;

	//���b�N������
	Direct3D::getInstance()->getContext()->Unmap(gradientbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�O���f�[�V�����p�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, gradientbuffer_.GetAddressOf());

	return true;
}

void SkyDome::renderShader(const int IndexCount)
{
	//���_���̓��C�A�E�g���Z�b�g
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�V�F�[�_�[���Z�b�g
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);

}

bool SkyDome::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC gradientbufferdesc;

	//�T�|�[�g�N���X�𐶐�
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	//�V�F�[�_�[�ǂݍ���
	hr = support_.get()->createVertexData(L"skydomevs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support_.get()->createPixelData(L"skydomeps.cso", pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//���_���̓��C�A�E�g���쐬
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

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
		return false;
	}

	//�O���f�[�V�����p�o�b�t�@�̐ݒ�
	gradientbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientbufferdesc.ByteWidth = sizeof(GradientBufferType);
	gradientbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientbufferdesc.MiscFlags = 0;
	gradientbufferdesc.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&gradientbufferdesc, NULL, gradientbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�O���f�[�V�����p�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	return true;
}

bool SkyDome::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	bool result;

	//�p�����[�^�[���Z�b�g
	result = setShaderParameters(World,View,Projection,ApexColor,CentorColor);
	if (!result)
	{
		Error::showDialog("�V�F�[�_�[�p�����[�^�[�̐ݒ�Ɏ��s");
		return false;
	}

	renderShader(Indexcount);

	return true;
}
