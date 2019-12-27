#include "stdafx.h"
#include "Depthshader.h"
#include"Direct3D.h"
#include"checkinputlayout.h"

Depthshader::Depthshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	matrixbuffer_ = nullptr;

}

Depthshader::~Depthshader()
{
}

bool Depthshader::init()
{
	bool result;
	result = initShader(L"depthvs.hlsl", L"depthps.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void Depthshader::destroy()
{
	destroyShader();
}

bool Depthshader::render(int indexCount, Matrix World, Matrix View, Matrix Projection)
{
	bool result;
	result = setShaderParameters(World, View, Projection);
	if (!result)
	{
		return false;
	}

	renderShader(indexCount);

	return true;
}

bool Depthshader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	HRESULT hr;
	ID3D10Blob* vertexshaderbuffer;
	ID3D10Blob* pixelshaderbuffer;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;

	pixelshaderbuffer = nullptr;
	vertexshaderbuffer = nullptr;

	hr = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer->GetBufferPointer(), pixelshaderbuffer->GetBufferSize(), nullptr, &pixelshader_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), nullptr, &vertexshader_);
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

	//���C�A�E�g���̗v�f�̐����擾
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

#ifdef _DEBUG
	if (!checkInputLayout(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif //_DEBUG

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

	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//�o�b�t�@�쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Depthshader::destroyShader()
{
	SAFE_RELEASE(matrixbuffer_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool Depthshader::setShaderParameters( Matrix World, Matrix View, Matrix Projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	unsigned int buffernumber;

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
	buffernumber = 0;

	//�X�V���ꂽ�l�Œ��_�V�F�[�_�̒萔�o�b�t�@���Ō�ɐݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, &matrixbuffer_);

	return true;
}

void Depthshader::renderShader(int Indexcount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//�����_�����O�Ɏg�p����V�F�[�_�[��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);

}
