#include "stdafx.h"
#include "Maskshader.h"
#include"Direct3D.h"

Maskshader::Maskshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	matrixbuffer_ = nullptr;
	samplestate_ = nullptr;
}

Maskshader::~Maskshader()
{
}

bool Maskshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"maskvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"maskps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	//作成されたデータをコピー
	vertexshader_ = support_.get()->getVertexShader();
	pixelshader_ = support_.get()->getPixelShader();

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

	//レイアウト内の要素の数を取得
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements);
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
		return false;
	}

	//作成した頂点入力レイアウトを取得
	layout_ = support_.get()->getInputLayout();

	//動的マトリックス定数バッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//このクラスから頂点シェーダの定数バッファにアクセスできるようにポインタを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//サンプラーの設定
	samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
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
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplestate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Maskshader::destroy()
{
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool Maskshader::render(int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	bool result;

	result = setShaderParameters(World, View, Projection, TextureArray);
	if (!result)
	{
		return false;
	}

	//シェーダーレンダリング
	rendershader(Indexcount);

	return true;
}

bool Maskshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	unsigned int buffnumber;

	//マトリックスを転置してシェーダー用に準備
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//書き込み可能なように定数バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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

	//定数バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//頂点シェーダで定数バッファの位置を設定
	buffnumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, matrixbuffer_.GetAddressOf());

	//ピクセルシェーダーでシェーダーリソーステクスチャ配列を設定
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 3, TextureArray);

	return true;
}

void Maskshader::rendershader(int Indexcount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//この三角形のレンダリングに使用される頂点シェーダとピクセルシェーダを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//サンプラー状態をピクセルシェーダーに設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplestate_);

	//三角形をレンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
