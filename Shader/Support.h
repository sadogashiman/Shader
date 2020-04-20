#pragma once

#include"error.h"
#include"release.h"


class Support
{
private:
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11ComputeShader* computeshader_;
	LPVOID vertexblob_;
	LPVOID pixelblob_;
	LPVOID computeblob_;
	size_t vertexsize_;
	size_t pixelsize_;
	size_t computesize_;
	ComPtr<ID3D10Blob> pixelshaderbuffer_;
	ComPtr<ID3D10Blob> vertexshaderbuffer_;
	ComPtr<ID3D10Blob> computeshaderbuffer_;
	ID3D11InputLayout* layout_;
	std::vector<char> vertexdataarray_;
	std::vector<char> pixeldataarray_;
	std::vector<char> computedataarray_;
	static wchar_t filename_[MAX_PATH];
	static wchar_t mtlfilename[MAX_PATH];

public:
	Support();
	~Support();

	//Check
	static bool checkInputLayoutData(const void* shadercode, size_t codesize,
								 const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutnum); //頂点入力レイアウトを作成するデータが有効か確認する関数
	//Search
	//ファイルパスが有効か判定（有効ならtrueを返す)
	static bool searchFile(const wchar_t* FileName);
	static bool searchFile(const char* FileName);
	static bool searchFile(const std::string FileName);

	//Rename
	static wchar_t* renameToImageFileName(const wchar_t* ModelFileName);			//渡されたファイル名の拡張子をディレクトリ内に存在する同名の画像ファイル拡張子に変更する
	static wchar_t* renameToMaterialFileName(const wchar_t* MaterialFileName);		//渡されたファイルパス文字列を拡張子.mtlに変換する
	
	//Create
	HRESULT createVertexData(const wchar_t* VertexShaderFileName);		 			//渡されたファイル名で頂点シェーダーを作成
	HRESULT createPixelData(const wchar_t* PixelShaderFileName);					//渡されたファイル名でピクセルシェーダーを作成
	HRESULT createComputeData(const wchar_t* ComputeShdaerFileName);
	HRESULT createVertexInputLayout(D3D11_INPUT_ELEMENT_DESC* PolygonLayoutArray,const unsigned int NumElements); //渡された情報で頂点入力レイアウトを作成(デバック時はデータチェックを行う)

	//Get
	inline const LPVOID getVertexBufferPtr()const { return vertexblob_; }			//頂点シェーダーのバッファポインタを返す
	inline const LPVOID getPixelBufferPtr()const { return pixelblob_; }				//ピクセルシェーダーのバッファポインタを返す
	inline const LPVOID getcomputeBufferPtr()const { return computeblob_; }			//コンピュートシェーダーのバッファポインタを返す
	inline const size_t getVertexBufferSize()const { return vertexsize_; }			//頂点シェーダーのバッファサイズを返す
	inline const size_t getPixelBufferSize()const { return pixelsize_; }			//ピクセルシェーダーのバッファサイズを返す
	inline const size_t getComputeBufferSize()const { return computesize_; }		//コンピュートシェーダーのバッファサイズを返す
	inline ID3D11VertexShader* getVertexShader()const { return vertexshader_; }		//作成された頂点シェーダーのポインターを返す
	inline ID3D11PixelShader* getPixelShader()const { return pixelshader_; }		//作成したピクセルシェーダーのポインターを返す
	inline ID3D11ComputeShader* getComputeShader()const { return computeshader_; }  //作成したコンピュートシェーダーのポインタを返す
	inline ID3D11InputLayout* getInputLayout()const { return layout_; }				//作成した頂点入力レイアウトのポインタを返す
};

