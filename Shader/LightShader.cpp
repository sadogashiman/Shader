#include "stdafx.h"
#include "Lightshader.h"
#include"Direct3D.h"
#include"release.h"


LightShader::LightShader()
{
	vertexshader_ = 0;
	pixelshader_ = 0;
	layout_ = 0;
	samplerstate_ = 0;
	matrixbuff_ = 0;
	lightbuffer_ = 0;
}

LightShader::~LightShader()
{
}

bool LightShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	//Support�N���X����
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("Support�N���X�̐����Ɏ��s");
		return false;
	}

	//�V�F�[�_�[�ǂݍ���
	hr = support_.get()->createVertexData(L"lightvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	hr = support_.get()->createPixelData(L"lightps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
		return false;
	}

	//�쐬���ꂽ�f�[�^���R�s�[
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

	//���C�A�E�g���̗v�f�̐����擾
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

#ifdef _DEBUG
	//�f�[�^���L�����m�F
	if (!Support::checkInputLayout(support_->getPixelBufferPtr(), support_->getPixelBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif // _DEBUG
	//���_���̓��C�A�E�g�̍쐬
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, support_->getPixelBufferPtr(), support_->getPixelBufferSize(), &layout_);
	if (FAILED(hr))
	{
		return false;
	}

	//�s�v�ɂȂ����f�[�^�̍폜
	support_.get()->destroyBufferData();

	//���I�}�g���b�N�X�萔�o�b�t�@�̐ݒ�
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//���̃N���X���璸�_�V�F�[�_�̒萔�o�b�t�@�ɃA�N�Z�X�ł���悤�Ƀ|�C���^���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuff_);
	if (FAILED(hr))
	{
		return false;
	}

	//�T���v���[�̐ݒ�
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

	//�e�N�X�`���̃T���v���[��Ԃ�ݒ�
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplerstate_);
	if (FAILED(hr))
	{
		return false;
	}

	//�s�N�Z���V�F�[�_�[�ɂ��郉�C�g�_�C�i�~�b�N�萔�o�b�t�@�̐ݒ�
	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	//���̃N���X�����璸�_�V�F�[�_�[�̒萔�o�b�t�@�ɃA�N�Z�X�ł���悤�Ƀ|�C���^���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, &lightbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void LightShader::destroy()
{
	//�e�I�u�W�F�N�g�̔j��
	SAFE_RELEASE(lightbuffer_);
	SAFE_RELEASE(matrixbuff_);
	SAFE_RELEASE(samplerstate_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool LightShader::render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* NormalTexture, Vector3 LightDirection)
{
	bool result;

	//�����_�����O�Ɋւ���V�F�[�_�[�p�����[�^��ݒ�
	result = SetShaderParameters(World, View, Projection,texture,NormalTexture,LightDirection);

	if (!result)
	{
		return false;
	}

	//���������o�b�t�@���V�F�[�_�[�Ń����_�����O
	renderShader(Indexcound);

	return true;
}

bool LightShader::SetShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* NormalTexture, Vector3 LightDirection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	unsigned int buffnumber;

	//�}�g���b�N�X��]�u���ăV�F�[�_�[�p�ɏ���
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//�������݉\�Ȃ悤�ɒ萔�o�b�t�@�����b�N
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuff_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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
	Direct3D::getInstance()->getContext()->Unmap(matrixbuff_, 0);

	//���_�V�F�[�_�Œ萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Œ��_�V�F�[�_�̒萔�o�b�t�@���Ō�ɐݒ�
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuff_);

	//�s�N�Z���V�F�[�_�[�Ńe�N�X�`�����\�[�X��ݒ�
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &texture);
	Direct3D::getInstance()->getContext()->PSSetShaderResources(1, 1, &NormalTexture);	//���C�g�萔�o�b�t�@�����b�N���ď������݉\��

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//�萔�o�b�t�@���̃f�[�^�ւ̃|�C���^���擾
	dataptr2 = (LightBufferType*)mappedresouce.pData;

	//�Ɩ��ϐ���萔�o�b�t�@�ɃR�s�[
	dataptr2->lightDirection = LightDirection;
	dataptr2->pading = 0.0F;

	//�萔�o�b�t�@�̃��b�N������
	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_, 0);

	//�s�N�Z���V�F�[�_�[�Ń��C�g�萔�o�b�t�@�̈ʒu��ݒ�
	buffnumber = 0;

	//�X�V���ꂽ�l�Ńs�N�Z���V�F�[�_�[�̃��C�g�萔�o�b�t�@��ݒ�
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, &lightbuffer_);

	return true;
}

void LightShader::renderShader(const int Indexcount)
{
	//���_���̓��C�A�E�g��ݒ�
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);
	//���̎O�p�`�̃����_�����O�Ɏg�p����钸�_�V�F�[�_�ƃs�N�Z���V�F�[�_��ݒ�
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//�T���v���[��Ԃ��s�N�Z���V�F�[�_�[�ɐݒ�
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplerstate_);

	//�O�p�`�������_�����O
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
