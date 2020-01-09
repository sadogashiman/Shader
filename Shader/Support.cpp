#include "stdafx.h"
#include "Support.h"
#include"Direct3D.h"

bool Support::checkInputLayout(const void* shadercode, size_t codesize, const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum)
{
	ID3D11ShaderReflection* vsref;
	HRESULT hr = D3DReflect(shadercode, codesize, IID_ID3D11ShaderReflection, (void**)&vsref);
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
	std::vector<char> vertexdataarray;
	vertexshaderbuffer = nullptr;

	//�R���p�C���ς݃V�F�[�_�[�t�@�C���W�J
	fp.open(VertexShaderFileName, std::ios::in | std::ios::binary);

	if (!fp.is_open())
	{
		char charfilename[MAX_PATH];
		wchar_t wcharfilename[MAX_PATH];

		//wchar����char�ɕϊ�
		wcstombs(charfilename, VertexShaderFileName, sizeof(VertexShaderFileName));

		const char* extension = ".hlsl";

		//�g���q��hlsl�ɕύX
		PathRenameExtension(charfilename, extension);

		//�ēxwchar�ɕϊ�
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//�V�F�[�_�[���R���p�C�����ă|�C���^���擾
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexshaderbuffer, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), NULL, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		vertexblob_ = vertexshaderbuffer->GetBufferPointer();
		vertexsize_ = vertexshaderbuffer->GetBufferSize();


	}
	else
	{
		//�|�C���^�𖖔���
		fp.seekg(0, std::ios::end);

		//�T�C�Y���擾
		size_t size = static_cast<size_t>(fp.tellg());

		//�z��T�C�Y��ύX
		vertexdataarray.resize(size);

		//�|�C���^�̈ʒu��擪��
		fp.seekg(0, std::ios::beg);

		//�ǂݍ���
		fp.read(&vertexdataarray[0], size);

		//�ǂݍ��ݏI��
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&vertexdataarray, size, nullptr, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�f�[�^�������o�ɃR�s�[
		vertexsize_ = size;
		vertexblob_ = &vertexdataarray[0];
	}

	return S_OK;
}

HRESULT Support::createPixelData(const wchar_t* PixelShaderFileName)
{
	HRESULT hr;
	std::ifstream fp;
	std::vector<char> pixeldataarray;
	pixelshaderbuffer = nullptr;

	//�R���p�C���ς݃V�F�[�_�[�t�@�C���W�J
	fp.open(PixelShaderFileName, std::ios::in | std::ios::binary);

	if (!fp.is_open())
	{
		char charfilename[MAX_PATH];
		wchar_t wcharfilename[MAX_PATH];

		//wchar����char�ɕϊ�
		wcstombs(charfilename, PixelShaderFileName, sizeof(PixelShaderFileName));

		const char* extension = ".hlsl";

		//�g���q��hlsl�ɕύX
		PathRenameExtension(charfilename, extension);

		//�ēxwchar�ɕϊ�
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//�V�F�[�_�[���R���p�C�����ă|�C���^���擾
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelshaderbuffer, NULL);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(pixelshaderbuffer->GetBufferPointer(), pixelshaderbuffer->GetBufferSize(), NULL, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		pixelblob_ = pixelshaderbuffer->GetBufferPointer();
		pixelsize_ = pixelshaderbuffer->GetBufferSize();
	}
	else
	{
		//�|�C���^�𖖔���
		fp.seekg(0, std::ios::end);

		//�T�C�Y���擾
		size_t size = static_cast<size_t>(fp.tellg());

		//�z��T�C�Y��ύX
		pixeldataarray.resize(size);

		//�|�C���^�̈ʒu��擪��
		fp.seekg(0, std::ios::beg);

		//�ǂݍ���
		fp.read(&pixeldataarray[0], size);

		//�ǂݍ��ݏI��
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&pixeldataarray, size, nullptr, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//�f�[�^�������o�ɃR�s�[
		pixelsize_ = size;
		pixelblob_ = &pixeldataarray[0];
	}

	return S_OK;
}

void Support::destroyBufferData()
{
	//�s�v�ɂȂ����f�[�^�̍폜
	pixelshaderbuffer->Release();
	pixelshaderbuffer = nullptr;

	vertexshaderbuffer->Release();
	vertexshaderbuffer = nullptr;
}
