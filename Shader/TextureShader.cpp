#include "stdafx.h"
#include "TextureShader.h"
#include"Direct3D.h"
#include"error.h"
#include"release.h"

Textureshader::Textureshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	matrixbuffer_ = nullptr;
	samplerstate_ = nullptr;
}

Textureshader::~Textureshader()
{
}


bool Textureshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbuffer;
	D3D11_SAMPLER_DESC samplerdesc;

	//Support�N���X����
	support_.reset(new Support);
	if (!support_.get())
	{
		return false;
	}

	//�V�F�[�_�[�쐬
	hr = support_.get()->createVertexData(L"texturevs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support_.get()->createPixelData(L"textureps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//�쐬���ꂽ�f�[�^���R�s�[
	vertexshader_ = support_.get()->getVertexShader();
	pixelshader_ = support_.get()->getPixelShader();

	//���_���̓��C�A�E�g�̍쐬
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

	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

#ifdef _DEBUG
	//�f�[�^���L�����m�F
	if (!Support::checkInputLayout(support_->getVertexBufferPtr(), support_->getVertexBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif // _DEBUG
	//���_���̓��C�A�E�g�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, support_->getVertexBufferPtr(), support_->getVertexBufferSize(), &layout_);
	if (FAILED(hr))
	{
		return false;
	}

	//�s�v�ɂȂ����f�[�^���폜
	support_.get()->destroyBufferData();

	//�R���X�^���g�o�b�t�@�̐ݒ�
	matrixbuffer.Usage = D3D11_USAGE_DYNAMIC;
	matrixbuffer.ByteWidth = sizeof(MatrixBufferType);
	matrixbuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbuffer.MiscFlags = 0;
	matrixbuffer.StructureByteStride = 0;

	//�R���X�^���g�o�b�t�@�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbuffer, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

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

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplerstate_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Textureshader::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	bool result;
	result = setShaderParameters(World, View, Projection, Texture);
	if (!result)
	{
		return false;
	}

	renderShader(Indexcount);

	return true;
}

void Textureshader::destroy()
{
	SAFE_RELEASE(samplerstate_);
	SAFE_RELEASE(matrixbuffer_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}


bool Textureshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	unsigned int buffernumber;

	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		return false;
	}

	//�V�F�[�_�[���ւ̃|�C���^�[���擾
	dataptr = (MatrixBufferType*)mappedresource.pData;

	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	buffernumber = 0;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, &matrixbuffer_);

	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);

	return true;
}

void Textureshader::renderShader(const int Indexcount)
{
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplerstate_);

	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
