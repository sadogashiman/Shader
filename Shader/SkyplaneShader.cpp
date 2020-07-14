#include "stdafx.h"
#include "SkyplaneShader.h"
#include"Direct3D.h"

bool SkyplaneShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	D3D11_SAMPLER_DESC samplerdesc;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC skybufferdesc;

	//サポートクラスを生成
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスの生成に失敗");
		return false;
	}

	//頂点シェーダーの作成
	hr = support_.get()->createVertexData(L"skyplane_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ピクセルシェーダーの作成
	hr = support_.get()->createPixelData(L"skyplane_ps.cso", pixelshader_.GetAddressOf());
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

	//頂点入力レイアウトの作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, _countof(polygonlayout), layout_.GetAddressOf());
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

	//サンプラーを作成
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, sampler_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("サンプラーの作成に失敗");
		return false;
	}

	//定数バッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//定数バッファの作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダー用定数バッファの作成に失敗");
		return false;
	}

	//ピクセルシェーダーよう定数バッファの作成
	skybufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	skybufferdesc.ByteWidth = sizeof(SkyBufferType);
	skybufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	skybufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	skybufferdesc.MiscFlags = 0;
	skybufferdesc.StructureByteStride = 0;

	//バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&skybufferdesc, NULL, skybuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダー用定数バッファの作成に失敗");
		return false;
	}

	return true;
}

bool SkyplaneShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, float Transition, float Scale, float Bright)
{
	bool result;

	//シェーダーパラメーターの設定
	result = setShaderParameters(World, View, Projection, Texture1, Texture2,Transition,Scale,Bright);
	if (!result)
	{
		Error::showDialog("シェーダーパラメーターの設定に失敗");
		return false;
	}

	//レンダリング
	renderShader(IndexCount);

	return true;
}

bool SkyplaneShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, float Transition, float Scale, float Bright)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	SkyBufferType* dataptr2;
	unsigned int buffernumber;

	//シェーダー用に行列を転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//定数バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//ポインタをキャスト
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//行列を定数バッファにコピー
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロック解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//定数バッファを更新
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//二番目のバッファのロック
	hr = Direct3D::getInstance()->getContext()->Map(skybuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//ポインタキャスト
	dataptr2 = (SkyBufferType*)mappedresource.pData;

	//定数バッファにコピー
	dataptr2->transition = Transition;
	dataptr2->scale = Scale;
	dataptr2->btight = Bright;
	dataptr2->padding = 0.0F;

	//ロック解除
	Direct3D::getInstance()->getContext()->Unmap(skybuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//ピクセルシェーダーの定数バッファを更新
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, skybuffer_.GetAddressOf());

	//シェーダーリソースを設定
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture1);
	Direct3D::getInstance()->getContext()->PSSetShaderResources(1, 1, &Texture2);

	return true;
}

void SkyplaneShader::renderShader(const int IndexCount)
{

	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//この三角形のレンダリングに使用される頂点シェーダとピクセルシェーダを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//サンプラー状態をピクセルシェーダーに設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, sampler_.GetAddressOf());

	//三角形をレンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);
}
