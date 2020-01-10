#include "stdafx.h"
#include "Support.h"
#include"Direct3D.h"

bool Support::checkInputLayout(const void* shadercode, size_t codesize, const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum)
{
	ID3D11ShaderReflection* vsref;
	HRESULT hr;
	hr = D3DReflect(shadercode, codesize, IID_ID3D11ShaderReflection, (void**)&vsref);
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_SHADER_DESC shaderdesc;
	hr = vsref->GetDesc(&shaderdesc);
	if (FAILED(hr))
	{
		vsref->Release();
		return false;
	}

	for (UINT i = 0; i < shaderdesc.InputParameters; ++i)
	{
		D3D11_SIGNATURE_PARAMETER_DESC desc;
		hr = vsref->GetInputParameterDesc(i, &desc);
		if (FAILED(hr))
		{
			vsref->Release();
			return false;
		}

		UINT j;

		for (j = 0; j < layoutnum; ++j)
		{
			if (strcmp(layout[j].SemanticName, desc.SemanticName) == 0)
			{
				break;
			}
		}
		if (j == layoutnum)
		{
			Error::showDialog("�V�F�[�_�[���ƃZ�}���e�B�N�X������v���܂���");
			vsref->Release();
			return false;
		}
	}

	vsref->Release();
	return true;
}

HRESULT Support::createVertexData(const wchar_t* VertexShaderFileName)
{
	HRESULT hr;
	std::ifstream fp;
	vertexshaderbuffer_ = nullptr;

	//�p�X���L�����m�F���邽�߂Ɉ�x�ϊ�
	char tmp[MAX_PATH];
	wcstombs(tmp, VertexShaderFileName, MAX_PATH);

	//�p�X���L�����m�F
	if (PathFileExists(tmp))
	{
		//�t�@�C���W�J
		fp.open(tmp,std::ios::binary);

		//�t�@�C���T�C�Y�擾
		size_t size = static_cast<size_t>(fp.seekg(0, std::ios::end).tellg());

		//�|�C���^�̈ʒu��擪��
		fp.seekg(0, std::ios::beg);

		//�z��T�C�Y��ύX
		vertexdataarray_.resize(size);

		//�ǂݍ���
		fp.read(&vertexdataarray_[0], size);

		//�ǂݍ��ݏI��
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&vertexdataarray_[0],size, nullptr, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�f�[�^�������o�ɃR�s�[
		vertexsize_ = size;
		vertexblob_ = &vertexdataarray_[0];
	}
	else
	{
		wchar_t wcharfilename[MAX_PATH] = L" ";

		//wchar����char�ɕϊ�
		wcstombs(tmp, VertexShaderFileName, MAX_PATH);

		//�g���q��hlsl�ɕύX
		PathRenameExtension(tmp, ".hlsl");

		//�ēxwchar�ɕϊ�
		mbstowcs(wcharfilename, tmp, sizeof(tmp));

		//�V�F�[�_�[���R���p�C�����ă|�C���^���擾
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexshaderbuffer_, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer_->GetBufferPointer(), vertexshaderbuffer_->GetBufferSize(), nullptr, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		vertexblob_ = vertexshaderbuffer_->GetBufferPointer();
		vertexsize_ = vertexshaderbuffer_->GetBufferSize();
	}

	return S_OK;
}

HRESULT Support::createPixelData(const wchar_t* PixelShaderFileName)
{
	HRESULT hr;
	std::ifstream fp;
	pixelshaderbuffer_ = nullptr;

	//�p�X���L�����m�F���邽�߂Ɉ�x�ϊ�
	char tmp[MAX_PATH];
	wcstombs(tmp, PixelShaderFileName, MAX_PATH);

	//�p�X���L�����m�F
	if (PathFileExists(tmp))
	{
		//�t�@�C���W�J
		fp.open(PixelShaderFileName, std::ios::binary);

		//�t�@�C���T�C�Y�擾
		size_t size = static_cast<size_t>(fp.seekg(0, std::ios::end).tellg());

		//�|�C���^�̈ʒu��擪��
		fp.seekg(0, std::ios::beg);

		//�z��T�C�Y��ύX
		pixeldataarray_.resize(size);

		//�ǂݍ���
		fp.read(&pixeldataarray_[0], size);

		//�ǂݍ��ݏI��
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(&pixeldataarray_[0], size, nullptr, &pixelshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�f�[�^�������o�ɃR�s�[
		pixelsize_ = size;
		pixelblob_ = &pixeldataarray_[0];
	}
	else
	{
		char charfilename[MAX_PATH] = " ";
		wchar_t wcharfilename[MAX_PATH] = L" ";

		//wchar����char�ɕϊ�
		wcstombs(charfilename, PixelShaderFileName, MAX_PATH);

		//�g���q��hlsl�ɕύX
		PathRenameExtension(charfilename, ".hlsl");

		//�ēxwchar�ɕϊ�
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//�V�F�[�_�[���R���p�C�����ă|�C���^���擾
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelshaderbuffer_, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer_->GetBufferPointer(), pixelshaderbuffer_->GetBufferSize(), nullptr, &pixelshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		pixelblob_ = pixelshaderbuffer_->GetBufferPointer();
		pixelsize_ = pixelshaderbuffer_->GetBufferSize();
	}

	return S_OK;
}

void Support::destroyBufferData()
{
	//�s�v�ɂȂ����f�[�^�̍폜
	SAFE_RELEASE(vertexshaderbuffer_);
	SAFE_RELEASE(pixelshaderbuffer_);
}