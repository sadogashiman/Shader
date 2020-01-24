#pragma once

#include"error.h"
#include"release.h"

//変換する拡張子

class Support
{
private:
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	ID3D10Blob* pixelshaderbuffer_;
	ID3D10Blob* vertexshaderbuffer_;
	std::vector<char> vertexdataarray_;
	std::vector<char> pixeldataarray_;

	enum Estate
	{
		kStateInit,
		kStateUpdate,
		kStateDelete,
		kStateMax,
	};


public:

	//Check
	static bool checkInputLayout(const void* shadercode, size_t codesize,
								 const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //頂点入力レイアウトを作成するデータが有効か確認する関数
	//Search
	static bool searchFile(const wchar_t* FileName);
	static bool searchFile(const char* FileName);
	static bool searchFile(const std::string FileName);

	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName);					//渡されたファイル名で頂点シェーダーを作成
	HRESULT createPixelData(const wchar_t* PixelShaderFileName);					//渡されたファイル名でピクセルシェーダーを作成

	//Get
	inline auto get() { return vertexblob_; }
	inline const LPVOID getVertexBufferPtr()const { return vertexblob_; }			//頂点シェーダーのバッファポインタを返す
	inline const LPVOID getPixelBufferPtr()const { return pixelblob_; }			//ピクセルシェーダーのバッファポインタを返す
	inline const size_t getVertexBufferSize()const { return vertexsize_; }			//頂点シェーダーのバッファサイズを返す
	inline const size_t getPixelBufferSize()const { return pixelsize_; }			//ピクセルシェーダーのバッファサイズを返す
	inline ID3D11VertexShader* getVertexShader()const { return vertexshader_; }		//作成された頂点シェーダーのポインターを返す
	inline ID3D11PixelShader* getPixelShader()const { return pixelshader_; }		//作成したピクセルシェーダーのポインターを返す

	//destroy
	void destroyBufferData();	//不要になったバッファデータを削除
};

