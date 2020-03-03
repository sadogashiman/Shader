#include "stdafx.h"
#include "FontShader.h"
#include"Direct3D.h"

FontShader::FontShader()
{
}

FontShader::~FontShader()
{
}

bool FontShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC pixelbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;


	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"fontvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"fontps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	//作成されたデータをコピー
	vertexshader_ = support_.get()->getVertexShader();
	pixelshader_ = support_.get()->getPixelShader();

	//頂点入力レイアウトを設定
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

	//要素数を計算
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout,numelements);
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
		return false;
	}

	//作成した頂点入力レイアウトを取得
	layout_ = support_.get()->getInputLayout();

	//マトリックスバッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//マトリックスバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		Error::showDialog("マトリックスバッファの作成に失敗");
		return false;
	}

	//サンプラーを作成
	samplerdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.MipLODBias = 0.0f;
	samplerdesc.MaxAnisotropy = 1;
	samplerdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerdesc.BorderColor[0] = 0;
	samplerdesc.BorderColor[1] = 0;
	samplerdesc.BorderColor[2] = 0;
	samplerdesc.BorderColor[3] = 0;
	samplerdesc.MinLOD = 0;
	samplerdesc.MaxLOD = D3D11_FLOAT32_MAX;

	//サンプラーを作成
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplestate_);
	if (FAILED(hr))
	{
		Error::showDialog("サンプラーの作成に失敗");
		return false;
	}

	//ピクセルバッファを設定
	pixelbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelbufferdesc.BindFlags = sizeof(PixelBufferType);
	pixelbufferdesc.ByteWidth = D3D11_BIND_CONSTANT_BUFFER;
	pixelbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelbufferdesc.MiscFlags = 0;
	pixelbufferdesc.StructureByteStride = 0;

	//ピクセルバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&pixelbufferdesc, NULL, &pixelbuffer_);
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルバッファの作成に失敗");
		return false;
	}
	
	return true;
}

bool FontShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color)
{
	bool result;

	//シェーダーのパラメーターを設定
	result = setShaderParameters(World, View, Projection, Texture, Color);
	if (!result)
	{
		Error::showDialog("シェーダーパラメーターの設定に失敗");
		return false;
	}

	renderShader(IndexCount);

	return true;
}

bool FontShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	PixelBufferType* dataptr2;
	unsigned int buffernumber;

	//シェーダーように行列を転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//バッファへのポインタを作成
	dataptr = (MatrixBufferType*)mappedresource.pData;

	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//バッファをセットする場所を設定
	buffernumber = 0;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, &matrixbuffer_);

	//テクスチャをシェーダーにセット
	Direct3D::getInstance()->getContext()->PSGetShaderResources(0, 1, &Texture);

	hr = Direct3D::getInstance()->getContext()->Map(pixelbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//バッファへのポインタを作成
	dataptr2 = (PixelBufferType*)mappedresource.pData;

	dataptr2->pixelcolor = Color;

	//ロック解除
	Direct3D::getInstance()->getContext()->Unmap(pixelbuffer_, 0);

	buffernumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, &pixelbuffer_);

	return true;
}

void FontShader::renderShader(const int IndexCount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//レンダリングするシェーダーを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//ピクセルシェーダーにサンプラーを設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplestate_);

	//フォントデータをレンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);
}