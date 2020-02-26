#include "stdafx.h"
#include "FontShader.h"
#include"Direct3D.h"
#include "Support.h"

FontShader::FontShader()
{
}

FontShader::~FontShader()
{
}

bool FontShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;
	Support support;

	//�V�F�[�_�[�ǂݍ���
	hr = support.createVertexData(L"fontvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support.createPixelData(L"fontps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//�쐬���ꂽ�f�[�^���R�s�[
	vertexshader_ = support.getVertexShader();
	pixelshader_ = support.getPixelShader();


	
	return true;
}

bool FontShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color)
{
	bool result;

	//�V�F�[�_�[�̃p�����[�^�[��ݒ�
	result = setShaderParameters(World, View, Projection, Texture, Color);
	if (!result)
	{
		Error::showDialog("�V�F�[�_�[�p�����[�^�[�̐ݒ�Ɏ��s");
		return false;
	}

	renderShader(IndexCount);

	return true;
}

bool FontShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	PixelBufferType* dataptr2;
	unsigned int buffernumber;

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

	//�o�b�t�@�ւ̃|�C���^���쐬
	dataptr = (MatrixBufferType*)mappedresource.pData;

	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//���b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//�o�b�t�@���Z�b�g����ꏊ��ݒ�
	buffernumber = 0;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//�e�N�X�`�����V�F�[�_�[�ɃZ�b�g
	Direct3D::getInstance()->getContext()->PSGetShaderResources(0, 1, &Texture);

	hr = Direct3D::getInstance()->getContext()->Map(pixelbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�o�b�t�@�ւ̃|�C���^���쐬
	dataptr2 = (PixelBufferType*)mappedresource.pData;

	dataptr2->pixelcolor = Color;

	//���b�N����
	Direct3D::getInstance()->getContext()->Unmap(pixelbuffer_.Get(), 0);

	buffernumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, pixelbuffer_.GetAddressOf());

	return true;
}

void FontShader::renderShader(const int IndexCount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//�����_�����O����V�F�[�_�[��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//�s�N�Z���V�F�[�_�[�ɃT���v���[��ݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplestate_.GetAddressOf());

	//�t�H���g�f�[�^�������_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);
}