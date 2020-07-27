#include "stdafx.h"
#include "Colorshader.h"


Colorshader::Colorshader()
{
	instanceptr_ = Direct3D::getInstance();
}

Colorshader::~Colorshader()
{
}

bool Colorshader::init()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC colorbufferdesc;
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygonlayout;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	hr = support_.get()->createVertexData(L"Shader/color_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/color_ps.cso", pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�z��T�C�Y�ύX
	polygonlayout.resize(1);

	//���_���̓��C�A�E�g�̍쐬
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	//���_���̓��C�A�E�g�̍쐬
	hr = support_.get()->createVertexInputLayout(polygonlayout,layout_.GetAddressOf());
	if (FAILED(hr))
	{
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
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�萔�o�b�t�@�̍쐬�Ɏ��s");
		return false;
	}

	//�J���[�o�b�t�@���쐬
	colorbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	colorbufferdesc.ByteWidth = sizeof(ColorBufferType);
	colorbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colorbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colorbufferdesc.MiscFlags = 0;
	colorbufferdesc.StructureByteStride = 0;

	//�J���[�o�b�t�@�̍쐬
	hr = instanceptr_->getDevice()->CreateBuffer(&colorbufferdesc, NULL, colorbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("�J���[�o�b�t�@�̍쐬�Ɏ��s");
	}

	return true;
}

bool Colorshader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, Vector4 Color)
{
	bool result;

	//�V�F�[�_�[�p�����[�^�[�̐ݒ�
	result = setShaderParameters(World, View, Projection,Color);
	if (!result)
	{
		Error::showDialog("�V�F�[�_�[�p�����[�^�[�̐ݒ�Ɏ��s");
		return false;
	}

	//�����_�����O
	renderShader(IndexCount);

	return true;
}

bool Colorshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 Color)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	ColorBufferType* dataptr2;
	unsigned int buffernumber;

	//�V�F�[�_�[�悤�ɍs���]�u
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�o�b�t�@�̃��b�N
	hr = instanceptr_->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
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
	instanceptr_->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//�X���b�g�ԍ���ݒ�
	buffernumber = 0;

	//�萔�o�b�t�@���Z�b�g
	instanceptr_->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//�s�N�Z���V�F�[�_�[�̏o�͐F������
	hr = instanceptr_->getContext()->Map(colorbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("�o�b�t�@�̃��b�N�Ɏ��s");
		return false;
	}

	//�|�C���^�L���X�g
	dataptr2 = (ColorBufferType*)mappedresource.pData;

	//�f�[�^�X�V
	dataptr2->color = Color;

	//���b�N����
	instanceptr_->getContext()->Unmap(colorbuffer_.Get(), 0);

	//�V�F�[�_�[�ɃZ�b�g
	instanceptr_->getContext()->PSSetConstantBuffers(0, 1, colorbuffer_.GetAddressOf());


	return true;
}

void Colorshader::renderShader(const int IndexCount)
{
	//���_���̓��C�A�E�g�̍쐬
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//�V�F�[�_�[�Z�b�g
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//�����_�����O
	instanceptr_->getContext()->DrawIndexed(IndexCount,0,0);
}