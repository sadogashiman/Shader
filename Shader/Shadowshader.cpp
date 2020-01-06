#include "stdafx.h"
#include "Shadowshader.h"
#include"Direct3D.h"
#include"checkinputlayout.h"


Shadowshader::Shadowshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	samplestateclamp_ = nullptr;
	matrixbuffer_ = nullptr;
	lightbuffer2_ = nullptr;
}

Shadowshader::~Shadowshader()
{
}

bool Shadowshader::init()
{
	bool result;
	result = initShader(L"shadowvs.hlsl", L"shadowps.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void Shadowshader::destroy()
{
	destroyShader();
}

bool Shadowshader::render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, const Matrix lightview, const Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor, Matrix Lightview2, Matrix Lightprojection2, ID3D11ShaderResourceView* Depthmap, Vector3 Lightposition2, Vector4 Diffusecolor2)
{
	bool result;

	//レンダリングに関するシェーダーパラメータを設定
	result = setShaderParameters(World, View, Projection, lightview, lightprojection, texture, Depthmaptexture, lightposition, Ambientcolor, Diffusecolor, Lightview2, Lightprojection2, Depthmap, Lightposition2, Diffusecolor2);

	if (!result)
	{
		return false;
	}

	//準備したバッファをシェーダーでレンダリング
	renderShader(Indexcound);

	return true;
}

bool Shadowshader::initShader(std::filesystem::path vsFileName, std::filesystem::path psFileName)
{
	HRESULT hr;
	ID3D10Blob* vertexshaderbuffer;
	ID3D10Blob* pixelshaderbuffer;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[3];
	unsigned int numelements;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc2;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	pixelshader_ = nullptr;
	vertexshader_ = nullptr;

	hr = D3DCompileFromFile(psFileName.c_str(), NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr = D3DCompileFromFile(vsFileName.c_str(), NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr =Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer->GetBufferPointer(), pixelshaderbuffer->GetBufferSize(), nullptr, &pixelshader_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), NULL, &vertexshader_);
	if (FAILED(hr))
	{
		return false;
	}

	//頂点入力レイアウトの設定
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	polygonlayout[1].SemanticName = "TEXCOORD";
	polygonlayout[1].SemanticIndex = 0;
	polygonlayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonlayout[1].InputSlot = 0;
	polygonlayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[1].InstanceDataStepRate = 0;

	polygonlayout[2].SemanticName = "NORMAL";
	polygonlayout[2].SemanticIndex = 0;
	polygonlayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[2].InputSlot = 0;
	polygonlayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[2].InstanceDataStepRate = 0;

	//レイアウト内の要素の数を取得
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);
#ifdef  _DEBUG
	if (!checkInputLayout(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif //  _DEBUG

	//頂点入力レイアウトの作成
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), &layout_);
	if (FAILED(hr))
	{
		return false;
	}

	//不要になったので削除
	vertexshaderbuffer->Release();
	vertexshaderbuffer = nullptr;
	pixelshaderbuffer->Release();
	pixelshaderbuffer = nullptr;


	//サンプラーの設定
	samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerdesc.MipLODBias = 0.0F;
	samplerdesc.MaxAnisotropy = 1;
	samplerdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerdesc.BorderColor[0] = 0;
	samplerdesc.BorderColor[1] = 0;
	samplerdesc.BorderColor[2] = 0;
	samplerdesc.BorderColor[3] = 0;
	samplerdesc.MinLOD = 0;
	samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;

	//テクスチャのサンプラー状態を設定
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplestateclamp_);
	if (FAILED(hr))
	{
		return false;
	}

	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplestatewrap_);
	if (FAILED(hr))
	{
		return false;
	}

	//定数バッファを設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, &lightbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	//頂点シェーダーにあるライトダイナミック定数バッファの設定
	lightbufferdesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc2.ByteWidth = sizeof(LightBufferType2);
	lightbufferdesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc2.MiscFlags = 0;
	lightbufferdesc2.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc2, NULL, &lightbuffer2_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Shadowshader::destroyShader()
{
	SAFE_RELEASE(samplestatewrap_);
	SAFE_RELEASE(lightbuffer_);
	SAFE_RELEASE(lightbuffer2_);
	SAFE_RELEASE(matrixbuffer_);
	SAFE_RELEASE(samplestateclamp_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool Shadowshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix Lightview, Matrix Lightprojection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 Lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor, Matrix Lightview2, Matrix LightProjection2, ID3D11ShaderResourceView* Depthmaptexture2, Vector3 Lightposition2, Vector4 Diffusecolor2)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	LightBufferType2* dataptr3;
	unsigned int buffnumber;

	//マトリックスを転置してシェーダー用に準備
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);
	Lightview = XMMatrixTranspose(Lightview);
	Lightprojection = XMMatrixTranspose(Lightprojection);
	Lightview2 = XMMatrixTranspose(Lightview2);
	LightProjection2 = XMMatrixTranspose(LightProjection2);

	//書き込み可能なように定数バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//定数バッファ内のデータへのポインタを取得
	dataptr = (MatrixBufferType*)mappedresouce.pData;

	//行列を定数バッファにコピー
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;
	dataptr->lightview = Lightview;
	dataptr->lightprojection = Lightprojection;
	dataptr->lightview2 = Lightview2;
	dataptr->lightprojection2 = LightProjection2;

	//定数バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//頂点シェーダで定数バッファの位置を設定
	buffnumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuffer_);

	//シェーダーリソースを設定
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);
	Direct3D::getInstance()->getContext()->PSGetShaderResources(1, 1, &Depthmaptexture);
	Direct3D::getInstance()->getContext()->PSSetShaderResources(2, 1, &Depthmaptexture2);

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_, 0, D3D11_MAP_WRITE_DISCARD,0, & mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	dataptr2 = (LightBufferType*)mappedresouce.pData;

	dataptr2->AmbientColor = Ambientcolor;
	dataptr2->DiffuseColor = Diffusecolor;
	dataptr2->DiffuseColor2 = Diffusecolor2;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_, 0);

	buffnumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, &lightbuffer_);

	//2番目のライト定数バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer2_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	dataptr3 = (LightBufferType2*)mappedresouce.pData;
	dataptr3->lightposition = Lightposition;
	dataptr3->lightposition2 = Lightposition2;
	dataptr3->padding1 = 0.0F;
	dataptr3->padding2 = 0.0F;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer2_, 0);

	buffnumber = 1;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &lightbuffer2_);

	return true;
}

void Shadowshader::renderShader(const int Indexcount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//レンダリングに使用するシェーダーを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//サンプラーの設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplestateclamp_);
	Direct3D::getInstance()->getContext()->PSSetSamplers(1, 1, &samplestatewrap_);

	//レンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
