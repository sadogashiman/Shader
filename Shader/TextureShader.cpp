#include "stdafx.h"
#include "TextureShader.h"
#include"Direct3D.h"

Textureshader::Textureshader()
{
	ZeroMemory(this, sizeof(Textureshader));
	instanceptr_ = Direct3D::getInstance();
}

Textureshader::~Textureshader()
{
}


bool Textureshader::init()
{
	HRESULT hr;
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygonlayout;
	D3D11_BUFFER_DESC matrixbuffer;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー作成
	hr = support_.get()->createVertexData(L"Shader/texture_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/texture_ps.cso", pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//配列サイズ変更
	polygonlayout.resize(2);

	//頂点入力レイアウトの作成
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

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, layout_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//マトリックスバッファの設定
	matrixbuffer.Usage = D3D11_USAGE_DYNAMIC;
	matrixbuffer.ByteWidth = sizeof(MatrixBufferType);
	matrixbuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbuffer.MiscFlags = 0;
	matrixbuffer.StructureByteStride = 0;

	//マトリックスバッファの作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbuffer, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

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

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplerstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Textureshader::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	bool result;
	result = setShaderParameters(World, View, Projection, Texture);
	if (!result)
	{
		return false;
	}

	renderShader(Indexcount);

	return true;
}

bool Textureshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	unsigned int buffernumber;

	//シェーダーように各行列を転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//定数バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		return false;
	}

	//シェーダー内へのポインターを取得
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//シェーダーに情報を渡す
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//定数バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//バッファの番号を設定
	buffernumber = 0;

	//頂点シェーダーに定数バッファをセット
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//ピクセルシェーダーにテクスチャ情報を渡す
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);

	return true;
}

void Textureshader::renderShader(const int Indexcount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//レンダリングに使用するシェーダーを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//サンプラーを設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplerstate_.GetAddressOf());

	//レンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
