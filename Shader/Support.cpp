#include "stdafx.h"
#include "Support.h"
#include"Direct3D.h"
#include"namespace.h"
wchar_t Support::filename_[MAX_PATH];
wchar_t Support::mtlfilename[MAX_PATH];

Support::Support()
{
}

Support::~Support()
{
}

bool Support::checkInputLayoutData(const void* shadercode, size_t codesize, const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum)
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

bool Support::searchFile(const wchar_t* FileName)
{
	char tmp[MAX_PATH];

	//wchar_t����char�ɕϊ�
	wcstombs(tmp, FileName, MAX_PATH);

	//�p�X���L�����m�F
	if (PathFileExists(tmp))
	{
		return true;
	}

	return false;
}

bool Support::searchFile(const char* FileName)
{
	//�p�X���L�����m�F
	if (PathFileExists(FileName))
	{
		return true;
	}

	return false;
}

bool Support::searchFile(const std::string FileName)
{
	if (PathFileExists(FileName.c_str()))
	{
		return true;
	}

	return false;
}

wchar_t* Support::renameToJPEG(const wchar_t* ModelFileName)
{
	//�����o�ɕ������ۑ�
	wcscpy(filename_, ModelFileName);

	char tmp[MAX_PATH] = " ";
	wcstombs(tmp, filename_, MAX_PATH);

	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		PathRenameExtension(tmp, Extension::kExtension[i]);

		//�L���ȃt�@�C���p�X���������ꍇ���[�v�𔲂���
		if (PathFileExists(tmp))
		{
			mbstowcs(filename_, tmp, MAX_PATH);
			break;
		}
	}

	return filename_;
}

wchar_t* Support::renameExtension(const wchar_t* SorcePath, const char* RenameExtension)
{
	char charfilename[MAX_PATH] = " ";

	//wchar����char�ɕϊ�
	wcstombs(charfilename, SorcePath, MAX_PATH);

	//�g���q��ϊ�
	PathRenameExtension(charfilename, RenameExtension);

	//�g���q�̕ϊ����I������̂�wchar�ɖ߂�
	mbstowcs(filename_ , charfilename, MAX_PATH);

	return filename_;
}

HRESULT Support::createVertexData(const wchar_t* VertexShaderFileName, ID3D11VertexShader** VertexShader)
{
	HRESULT hr;
	std::ifstream fp;
	vertexshaderbuffer_ = nullptr;

	//�p�X���L�����m�F
	if (searchFile(VertexShaderFileName))
	{
		//�t�@�C���W�J
		fp.open(VertexShaderFileName, std::ios::binary);

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

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&vertexdataarray_[0], size, nullptr, VertexShader);
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
		wchar_t* wcharfilename;

		wcharfilename = renameExtension(VertexShaderFileName, ".hlsl");

		//�V�F�[�_�[���R���p�C�����ă|�C���^���擾
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, vertexshaderbuffer_.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			Error::showDialog("���_�V�F�[�_�[�̓��I�R���p�C���Ɏ��s");
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer_.Get()->GetBufferPointer(), vertexshaderbuffer_.Get()->GetBufferSize(), nullptr, VertexShader);
		if (FAILED(hr))
		{
			Error::showDialog("���_�V�F�[�_�[�̍쐬�Ɏ��s");
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		vertexblob_ = vertexshaderbuffer_.Get()->GetBufferPointer();
		vertexsize_ = vertexshaderbuffer_.Get()->GetBufferSize();
	}

	return S_OK;
}

HRESULT Support::createPixelData(const wchar_t* PixelShaderFileName, ID3D11PixelShader** PixelShader)
{
	HRESULT hr;
	std::ifstream fp;
	pixelshaderbuffer_ = nullptr;

	//�p�X���L�����m�F
	if (searchFile(PixelShaderFileName))
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

		hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(&pixeldataarray_[0], size, nullptr, PixelShader);
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
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, pixelshaderbuffer_.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			Error::showDialog("�s�N�Z���V�F�[�_�[�̓��I�R���p�C���Ɏ��s");
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer_.Get()->GetBufferPointer(), pixelshaderbuffer_.Get()->GetBufferSize(), nullptr, PixelShader);
		if (FAILED(hr))
		{
			Error::showDialog("�s�N�Z���V�F�[�_�[�̍쐬�Ɏ��s");
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		pixelblob_ = pixelshaderbuffer_.Get()->GetBufferPointer();
		pixelsize_ = pixelshaderbuffer_.Get()->GetBufferSize();
	}

	return S_OK;
}

HRESULT Support::createComputeData(const wchar_t* ComputeShaderFileName, ID3D11ComputeShader** ComputeShader)
{
	HRESULT hr;
	std::ifstream fp;
	computeshaderbuffer_ = nullptr;

	//�p�X���L�����m�F
	if (searchFile(ComputeShaderFileName))
	{
		//�t�@�C���W�J
		fp.open(ComputeShaderFileName, std::ios::binary);

		//�t�@�C���T�C�Y�擾
		size_t size = static_cast<size_t>(fp.seekg(0, std::ios::end).tellg());

		//�|�C���^�̈ʒu��擪��
		fp.seekg(0, std::ios::beg);

		//�z��T�C�Y��ύX
		computedataarray_.resize(size);

		//�ǂݍ���
		fp.read(&computedataarray_[0], size);

		//�ǂݍ��ݏI��
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateComputeShader(&computedataarray_[0], size, nullptr, ComputeShader);
		if (FAILED(hr))
		{
			return hr;
		}

		//�f�[�^�������o�ɃR�s�[
		computesize_ = size;
		computeblob_ = &computedataarray_[0];
	}
	else
	{
		char charfilename[MAX_PATH] = " ";
		wchar_t wcharfilename[MAX_PATH] = L" ";

		//wchar����char�ɕϊ�
		wcstombs(charfilename, ComputeShaderFileName, MAX_PATH);

		//�g���q��hlsl�ɕύX
		PathRenameExtension(charfilename, ".hlsl");

		//�ēxwchar�ɕϊ�
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//�V�F�[�_�[���R���p�C�����ă|�C���^���擾
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, computeshaderbuffer_.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			Error::showDialog("�R���s���[�g�V�F�[�_�[�̓��I�R���p�C���Ɏ��s");
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateComputeShader(computeshaderbuffer_.Get()->GetBufferPointer(), computeshaderbuffer_.Get()->GetBufferSize(), nullptr, ComputeShader);
		if (FAILED(hr))
		{
			Error::showDialog("�R���s���[�g�V�F�[�_�[�̍쐬�Ɏ��s");
			return hr;
		}

		//�o�b�t�@�T�C�Y�ƃ|�C���^���R�s�[
		computeblob_ = computeshaderbuffer_.Get()->GetBufferPointer();
		computesize_ = computeshaderbuffer_.Get()->GetBufferSize();
	}

	return S_OK;
}

HRESULT Support::createVertexInputLayout(D3D11_INPUT_ELEMENT_DESC* PolygonLayoutArray, const unsigned int NumElements, ID3D11InputLayout** InputLayout)
{
	HRESULT hr;
#ifdef _DEBUG
	if (!(checkInputLayoutData(vertexblob_, vertexsize_, PolygonLayoutArray, NumElements)))
	{
		Error::showDialog("���_���̓��C�A�E�g�̃f�[�^������������܂���");
		return S_FALSE;
	}
#endif // _DEBUG
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(PolygonLayoutArray, NumElements, vertexblob_, vertexsize_, InputLayout);
	if (FAILED(hr))
	{
		Error::showDialog("���_���̓��C�A�E�g�̍쐬�Ɏ��s");
		return hr;
	}

	return S_OK;
}
