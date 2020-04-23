#pragma once
#include"Support.h"
#include"Light.h"
class Ray_trace_HW
{
private:
	struct StructuredElement
	{
		 Vector3 startpoint;
		 Vector3 direction;
		 Vector3 intersection;
		 Vector3 normal;
		 float distance;
		 DWORD color;
	};

	struct ConstantBufferType
	{
		ALIGN16 Vector3 position;
		ALIGN16 float r; //半径
		ALIGN16 Matrix sphereiw; //球のワールド逆行列
		ALIGN16 Matrix miw; //カメラ逆行列
		ALIGN16 Vector3 lightposition; //ライト座標
	};

	struct Sphere
	{
		Vector3 position; //中心座標
		float r; //半径
		Matrix im; //ワールドの逆行列
	};

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11ComputeShader* computeshader_;
	ID3D11InputLayout* layout_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	ComPtr<ID3D11Texture2D> cpdynamictexture_;
	ID3D11ShaderResourceView* dynamictexture_;
	Vector3 eyepos_;
	unsigned int numsphere_;
	Sphere* spheresrray_[100];


	std::unique_ptr<Support> support_;

	//コンピュートシェーダー用
	ComPtr<ID3D11Buffer> cpstructuredbuffer_;
	ComPtr<ID3D11Buffer> cpbufferinput_;       //シェーダー内のレイデータ参照用
	ComPtr<ID3D11Buffer> cpbufferresult_;	   //シェーダーからの結果を受け取る用
	ComPtr<ID3D11Buffer> cpconstantbuffer_;	   //レイを渡すためのバッファ
	ComPtr<ID3D11ShaderResourceView> cpbufferinputsrv_; //シェーダー内のレイデータ参照用
	ComPtr<ID3D11UnorderedAccessView> cpbufferresultuav_; //結果を受け取るための変数


	//関数
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection,Light* Light);
	void renderShader(const int IndexCount);
	Vector3 transScreenToWorld(const int ScreenHeight, const int ScreenWidth); //スクリーン座標をワールド座標に変換
	void addSphere(Sphere* Sp);
public:
	Ray_trace_HW();
	~Ray_trace_HW();
	bool init();
	bool render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Light* Light);
	void destroy();
};

