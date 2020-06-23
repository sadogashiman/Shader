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
			Error::showDialog("シェーダー側とセマンティクス名が一致しません");
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

	//wchar_tからcharに変換
	wcstombs(tmp, FileName, MAX_PATH);

	//パスが有効か確認
	if (PathFileExists(tmp))
	{
		return true;
	}

	return false;
}

bool Support::searchFile(const char* FileName)
{
	//パスが有効か確認
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
	//メンバに文字列を保存
	wcscpy(filename_, ModelFileName);

	char tmp[MAX_PATH] = " ";
	wcstombs(tmp, filename_, MAX_PATH);

	for (int i = 0; i < kExtensionTypeNum; i++)
	{
		PathRenameExtension(tmp, Extension::kExtension[i]);

		//有効なファイルパスを見つけた場合ループを抜ける
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

	//wcharからcharに変換
	wcstombs(charfilename, SorcePath, MAX_PATH);

	//拡張子を変換
	PathRenameExtension(charfilename, RenameExtension);

	//拡張子の変換が終わったのでwcharに戻す
	mbstowcs(filename_ , charfilename, MAX_PATH);

	return filename_;
}

HRESULT Support::createVertexData(const wchar_t* VertexShaderFileName, ID3D11VertexShader** VertexShader)
{
	HRESULT hr;
	std::ifstream fp;
	vertexshaderbuffer_ = nullptr;

	//パスが有効か確認
	if (searchFile(VertexShaderFileName))
	{
		//ファイル展開
		fp.open(VertexShaderFileName, std::ios::binary);

		//ファイルサイズ取得
		size_t size = static_cast<size_t>(fp.seekg(0, std::ios::end).tellg());

		//ポインタの位置を先頭に
		fp.seekg(0, std::ios::beg);

		//配列サイズを変更
		vertexdataarray_.resize(size);

		//読み込み
		fp.read(&vertexdataarray_[0], size);

		//読み込み終了
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(&vertexdataarray_[0], size, nullptr, VertexShader);
		if (FAILED(hr))
		{
			return hr;
		}

		//データをメンバにコピー
		vertexsize_ = size;
		vertexblob_ = &vertexdataarray_[0];

	}
	else
	{
		wchar_t* wcharfilename;

		wcharfilename = renameExtension(VertexShaderFileName, ".hlsl");

		//シェーダーをコンパイルしてポインタを取得
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, vertexshaderbuffer_.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			Error::showDialog("頂点シェーダーの動的コンパイルに失敗");
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer_.Get()->GetBufferPointer(), vertexshaderbuffer_.Get()->GetBufferSize(), nullptr, VertexShader);
		if (FAILED(hr))
		{
			Error::showDialog("頂点シェーダーの作成に失敗");
			return hr;
		}

		//バッファサイズとポインタをコピー
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

	//パスが有効か確認
	if (searchFile(PixelShaderFileName))
	{
		//ファイル展開
		fp.open(PixelShaderFileName, std::ios::binary);

		//ファイルサイズ取得
		size_t size = static_cast<size_t>(fp.seekg(0, std::ios::end).tellg());

		//ポインタの位置を先頭に
		fp.seekg(0, std::ios::beg);

		//配列サイズを変更
		pixeldataarray_.resize(size);

		//読み込み
		fp.read(&pixeldataarray_[0], size);

		//読み込み終了
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(&pixeldataarray_[0], size, nullptr, PixelShader);
		if (FAILED(hr))
		{
			return hr;
		}

		//データをメンバにコピー
		pixelsize_ = size;
		pixelblob_ = &pixeldataarray_[0];
	}
	else
	{
		char charfilename[MAX_PATH] = " ";
		wchar_t wcharfilename[MAX_PATH] = L" ";

		//wcharからcharに変換
		wcstombs(charfilename, PixelShaderFileName, MAX_PATH);

		//拡張子をhlslに変更
		PathRenameExtension(charfilename, ".hlsl");

		//再度wcharに変換
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//シェーダーをコンパイルしてポインタを取得
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, pixelshaderbuffer_.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			Error::showDialog("ピクセルシェーダーの動的コンパイルに失敗");
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer_.Get()->GetBufferPointer(), pixelshaderbuffer_.Get()->GetBufferSize(), nullptr, PixelShader);
		if (FAILED(hr))
		{
			Error::showDialog("ピクセルシェーダーの作成に失敗");
			return hr;
		}

		//バッファサイズとポインタをコピー
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

	//パスが有効か確認
	if (searchFile(ComputeShaderFileName))
	{
		//ファイル展開
		fp.open(ComputeShaderFileName, std::ios::binary);

		//ファイルサイズ取得
		size_t size = static_cast<size_t>(fp.seekg(0, std::ios::end).tellg());

		//ポインタの位置を先頭に
		fp.seekg(0, std::ios::beg);

		//配列サイズを変更
		computedataarray_.resize(size);

		//読み込み
		fp.read(&computedataarray_[0], size);

		//読み込み終了
		fp.close();

		hr = Direct3D::getInstance()->getDevice()->CreateComputeShader(&computedataarray_[0], size, nullptr, ComputeShader);
		if (FAILED(hr))
		{
			return hr;
		}

		//データをメンバにコピー
		computesize_ = size;
		computeblob_ = &computedataarray_[0];
	}
	else
	{
		char charfilename[MAX_PATH] = " ";
		wchar_t wcharfilename[MAX_PATH] = L" ";

		//wcharからcharに変換
		wcstombs(charfilename, ComputeShaderFileName, MAX_PATH);

		//拡張子をhlslに変更
		PathRenameExtension(charfilename, ".hlsl");

		//再度wcharに変換
		mbstowcs(wcharfilename, charfilename, sizeof(charfilename));

		//シェーダーをコンパイルしてポインタを取得
		hr = D3DCompileFromFile(wcharfilename, NULL, NULL, "main", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, computeshaderbuffer_.GetAddressOf(), NULL);
		if (FAILED(hr))
		{
			Error::showDialog("コンピュートシェーダーの動的コンパイルに失敗");
			return hr;
		}

		hr = Direct3D::getInstance()->getDevice()->CreateComputeShader(computeshaderbuffer_.Get()->GetBufferPointer(), computeshaderbuffer_.Get()->GetBufferSize(), nullptr, ComputeShader);
		if (FAILED(hr))
		{
			Error::showDialog("コンピュートシェーダーの作成に失敗");
			return hr;
		}

		//バッファサイズとポインタをコピー
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
		Error::showDialog("頂点入力レイアウトのデータが正しくありません");
		return S_FALSE;
	}
#endif // _DEBUG
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(PolygonLayoutArray, NumElements, vertexblob_, vertexsize_, InputLayout);
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
		return hr;
	}

	return S_OK;
}
