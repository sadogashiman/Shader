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

	//コンパイル済みシェーダーファイル展開
	fp.open(VertexShaderFileName, std::ios::in | std::ios::binary);

	if (!fp.is_open())
	{
		char charfilename[MAX_PATH];
		wchar_t wcharfilename[MAX_PATH];

		//wcharからcharに変換
		wcstombs(charfilename, VertexShaderFileName, sizeof(VertexShaderFileName));

		const char* extension = ".hlsl";

		//拡張子をhlslに変更
		PathRenameExtension(charfilename, extension);

		//再度wcharに変換
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//シェーダーをコンパイルしてポインタを取得
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

		//バッファサイズとポインタをコピー
		vertexblob_ = vertexshaderbuffer->GetBufferPointer();
		vertexsize_ = vertexshaderbuffer->GetBufferSize();


	}
	else
	{
		//ポインタを末尾へ
		fp.seekg(0, std::ios::end);

		//サイズを取得
		size_t size = static_cast<size_t>(fp.tellg());

		//配列サイズを変更
		vertexdataarray.resize(size);

		//ポインタの位置を先頭に
		fp.seekg(0, std::ios::beg);

		//読み込み
		fp.read(&vertexdataarray[0], size);

		//読み込み終了
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&vertexdataarray, size, nullptr, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//データをメンバにコピー
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

	//コンパイル済みシェーダーファイル展開
	fp.open(PixelShaderFileName, std::ios::in | std::ios::binary);

	if (!fp.is_open())
	{
		char charfilename[MAX_PATH];
		wchar_t wcharfilename[MAX_PATH];

		//wcharからcharに変換
		wcstombs(charfilename, PixelShaderFileName, sizeof(PixelShaderFileName));

		const char* extension = ".hlsl";

		//拡張子をhlslに変更
		PathRenameExtension(charfilename, extension);

		//再度wcharに変換
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//シェーダーをコンパイルしてポインタを取得
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

		//バッファサイズとポインタをコピー
		pixelblob_ = pixelshaderbuffer->GetBufferPointer();
		pixelsize_ = pixelshaderbuffer->GetBufferSize();
	}
	else
	{
		//ポインタを末尾へ
		fp.seekg(0, std::ios::end);

		//サイズを取得
		size_t size = static_cast<size_t>(fp.tellg());

		//配列サイズを変更
		pixeldataarray.resize(size);

		//ポインタの位置を先頭に
		fp.seekg(0, std::ios::beg);

		//読み込み
		fp.read(&pixeldataarray[0], size);

		//読み込み終了
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&pixeldataarray, size, nullptr, &vertexshader_);
		if (FAILED(hr))
		{
			return hr;
		}

		//データをメンバにコピー
		pixelsize_ = size;
		pixelblob_ = &pixeldataarray[0];
	}

	return S_OK;
}

void Support::destroyBufferData()
{
	//不要になったデータの削除
	pixelshaderbuffer->Release();
	pixelshaderbuffer = nullptr;

	vertexshaderbuffer->Release();
	vertexshaderbuffer = nullptr;
}
