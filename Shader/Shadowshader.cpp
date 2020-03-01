#include "stdafx.h"
#include "Shadowshader.h"
#include"Direct3D.h"


Shadowshader::Shadowshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	samplestateclamp_ = nullptr;
	samplestatewrap_ = nullptr;
	matrixbuffer_ = nullptr;
	lightbuffer_ = nullptr;
	lightbuffer2_ = nullptr;
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
	D3D11_BUFFER_DESC lightbufferdesc2;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("�T�|�[�g�N���X�̃������m�ۂɎ��s");
		return false;
	}

	//�V�F�[�_�[�ǂݍ���
	hr = support_.get()->createVertexData(L"shadowvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support_.get()->createPixelData(L"shadowps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//�R���p�C���ς݃V�F�[�_�[���擾
	vertexshader_ = support_.get()->getVertexShader();
	pixelshader_ = support_.get()->getPixelShader();

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

#ifdef _DEBUG
	//�f�[�^���L�����m�F
	if (!Support::checkInputLayoutData(support_.get()->getVertexBufferPtr(), support_.get()->getVertexBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif // _DEBUG
	//���_���̓��C�A�E�g�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, support_.get()->getVertexBufferPtr(), support_.get()->getVertexBufferSize(), &layout_);
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

	//�e�N�X�`���̃T���v���[��Ԃ�ݒ�
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplestateclamp_);
	if (FAILED(hr))
	{
		return false;
	}

	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplestatewrap_);
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

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, &lightbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	//���_�V�F�[�_�[�ɂ��郉�C�g�_�C�i�~�b�N�萔�o�b�t�@�̐ݒ�
	lightbufferdesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc2.ByteWidth = sizeof(LightBufferType2);
	lightbufferdesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc2.MiscFlags = 0;
	lightbufferdesc2.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc2, NULL, &lightbuffer2_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Shadowshader::destroy()
{
	SAFE_RELEASE(samplestatewrap_);
	SAFE_RELEASE(lightbuffer_);
	SAFE_RELEASE(lightbuffer2_);
	SAFE_RELEASE(matrixbuffer_);
	SAFE_RELEASE(samplestateclamp_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool Shadowshader::render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, const Matrix lightview, const Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor)
{	bool result;

	//�����_�����O�Ɋւ���V�F�[�_�[�p�����[�^��ݒ�
	result = setShaderParameters(World, View, Projection, lightview, lightprojection, texture, Depthmaptexture, lightposition, Ambientcolor, Diffusecolor);

	if (!result)
	{
		return false;
	}

	//���������o�b�t�@���V�F�[�_�[�Ń����_�����O
	renderShader(Indexcound);

	return true;
}

bool Shadowshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix Lightview, Matrix Lightprojection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 Lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor)
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
	dataptr->lightview = Lightview;
	dataptr->lightprojection = Lightprojection;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//���_�V�F�[�_�Œ萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Œ��_�V�F�[�_�̒萔�o�b�t�@���Ō�ɐݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuffer_);

	//�V�F�[�_�[���\�[�X��ݒ�
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);
	Direct3D::getInstance()->getContext()->PSGetShaderResources(1, 1, &Depthmaptexture);

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_, 0, D3D11_MAP_WRITE_DISCARD,0, & mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	dataptr2 = (LightBufferType*)mappedresouce.pData;

	dataptr2->AmbientColor = Ambientcolor;
	dataptr2->DiffuseColor = Diffusecolor;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_, 0);

	buffnumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, &lightbuffer_);

	//2�Ԗڂ̃��C�g�萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer2_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//�V�F�[�_�[���̃f�[�^���X�V
	dataptr3 = (LightBufferType2*)mappedresouce.pData;
	dataptr3->lightposition = Lightposition;
	dataptr3->padding1 = 0.0F;

	//���b�N������
	Direct3D::getInstance()->getContext()->Unmap(lightbuffer2_, 0);

	//�o�b�t�@�ԍ���ݒ�
	buffnumber = 1;

	//���_�V�F�[�_�[�ɒ萔�o�b�t�@���Z�b�g
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &lightbuffer2_);

	return true;
}

void Shadowshader::renderShader(const int Indexcount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//�����_�����O�Ɏg�p����V�F�[�_�[��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//�T���v���[�̐ݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplestateclamp_);
	Direct3D::getInstance()->getContext()->PSSetSamplers(1, 1, &samplestatewrap_);

	//�����_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
