#pragma once

#include"error.h"
#include"release.h"
#include"Model.h"

class Support
{
private:
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	LPVOID computeblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	size_t computesize_;
	ComPtr<ID3D10Blob> pixelshaderbuffer_;
	ComPtr<ID3D10Blob> vertexshaderbuffer_;
	ComPtr<ID3D10Blob> computeshaderbuffer_;
	std::vector<char> vertexdataarray_;
	std::vector<char> pixeldataarray_;
	std::vector<char> computedataarray_;
	static wchar_t filename_[MAX_PATH];

public:
	Support();
	~Support();

	//Check
	static bool checkInputLayoutData(const void* shadercode, size_t codesize,const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //頂点入力レイアウトを作成するデータが有効か確認する関数
	
	//Search
	//ファイルパスが有効か判定（有効ならtrueを返す)
	static bool searchFile(const wchar_t* FileName);
	static bool searchFile(const char* FileName);
	static bool searchFile(const std::string FileName);

	//Rename
	static wchar_t* renameExtension(const wchar_t* SorcePath,const char* RenameExtension); //渡されたファイルパスの拡張子を変換する関数 ※拡張子は[.jpg]のようにドットを付ける
	
	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName,ID3D11VertexShader** VertexShader);		 			                          //渡されたファイル名で頂点シェーダーを作成
	HRESULT createPixelData(const wchar_t* PixelShaderFileName,ID3D11PixelShader** PixelShader);								                  //渡されたファイル名でピクセルシェーダーを作成
	HRESULT createComputeData(const wchar_t* ComputeShdaerFileName,ID3D11ComputeShader** ComputeShader);										  //渡されたファイル名でコンピュートシェーダーを作成
	HRESULT createVertexInputLayout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& PolygonLayoutArray, ID3D11InputLayout** InputLayout);
	//Get
	inline const LPVOID getVertexBufferPtr()const { return vertexblob_; }			//頂点シェーダーのバッファポインタを返す
	inline const LPVOID getPixelBufferPtr()const { return pixelblob_; }				//ピクセルシェーダーのバッファポインタを返す
	inline const LPVOID getComputeBufferPtr()const { return computeblob_; }			//コンピュートシェーダーのバッファポインタを返す
	inline const size_t getVertexBufferSize()const { return vertexsize_; }			//頂点シェーダーのバッファサイズを返す
	inline const size_t getPixelBufferSize()const { return pixelsize_; }			//ピクセルシェーダーのバッファサイズを返す
	inline const size_t getComputeBufferSize()const { return computesize_; }		//コンピュートシェーダーのバッファサイズを返す
};

