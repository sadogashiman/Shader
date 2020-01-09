#pragma once
#include<Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include"error.h"

class Support
{
private:
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	ID3D10Blob* pixelshaderbuffer;
	ID3D10Blob* vertexshaderbuffer;


public:

	//check
	static bool checkInputLayout(const void* shadercode, size_t codesize,
						  const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //頂点入力レイアウトを作成するデータが有効か確認する関数

	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName);			//渡されたファイル名で頂点シェーダーを作成
	HRESULT createPixelData(const wchar_t* PixelShaderFileName);			//渡されたファイル名でピクセルシェーダーを作成

	//Get
	inline const LPVOID* getVertexBufferPtr()const { return &vertexblob_; }		//頂点シェーダーのバッファポインタを返す
	inline const LPVOID* getPixelBufferPtr()const { return &pixelblob_; }		//ピクセルシェーダーのバッファポインタを返す
	inline const size_t getVertexBufferSize()const { return vertexsize_; }		//頂点シェーダーのバッファサイズを返す
	inline const size_t getPixelBufferSize()const { return pixelsize_; }		//ピクセルシェーダーのバッファサイズを返す
	inline ID3D11VertexShader* getVertexShader()const { return vertexshader_; }	//作成された頂点シェーダーのポインターを返す
	inline ID3D11PixelShader* getPixelShader()const { return pixelshader_; }		//作成したピクセルシェーダーのポインターを返す

	//destroy
	void destroyBufferData();	//不要になったバッファデータを削除
};

