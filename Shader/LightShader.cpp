#include "stdafx.h"
#include "LightShader.h"


LightShader::LightShader()
{
	ZeroMemory(this, sizeof(LightShader));
	instanceptr_ = Direct3D::getInstance();
}

LightShader::~LightShader()
{
}

bool LightShader::init()
{
	HRESULT hr;
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygonlayout;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"Shader/light_vs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/light_ps.cso",pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//配列サイズ変更
	polygonlayout.resize(3);

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

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, layout_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

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
	hr = instanceptr_->getDevice()->CreateSamplerState(&samplerdesc, samplerstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//動的マトリックス定数バッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//このクラスから頂点シェーダの定数バッファにアクセスできるようにポインタを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ピクセルシェーダーにあるライトダイナミック定数バッファの設定
	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	//このクラス内から頂点シェーダーの定数バッファにアクセスできるようにポインタを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&lightbufferdesc, NULL, lightbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool LightShader::render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* NormalTexture, Vector3 LightDirection)
{
	bool result;

	//レンダリングに関するシェーダーパラメータを設定
	result = SetShaderParameters(World, View, Projection,Texture,NormalTexture,LightDirection);
	if (!result)
	{
		return false;
	}

	//準備したバッファをシェーダーでレンダリング
	renderShader(Indexcound);

	return true;
}

bool LightShader::SetShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* NormalTexture, Vector3 LightDirection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	unsigned int buffnumber;

	//マトリックスを転置してシェーダー用に準備
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//書き込み可能なように定数バッファをロック
	hr = instanceptr_->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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
	instanceptr_->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//頂点シェーダで定数バッファの位置を設定
	buffnumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	instanceptr_->getContext()->VSSetConstantBuffers(buffnumber, 1, matrixbuffer_.GetAddressOf());

	//ピクセルシェーダーでテクスチャリソースを設定
	instanceptr_->getContext()->PSSetShaderResources(0, 1, &Texture);
	instanceptr_->getContext()->PSSetShaderResources(1, 1, &NormalTexture);	//ライト定数バッファをロックして書き込み可能に

	hr = instanceptr_->getContext()->Map(lightbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//定数バッファ内のデータへのポインタを取得
	dataptr2 = (LightBufferType*)mappedresouce.pData;

	//照明変数を定数バッファにコピー
	dataptr2->lightDirection = LightDirection;
	dataptr2->pading = 0.0F;

	//定数バッファのロックを解除
	instanceptr_->getContext()->Unmap(lightbuffer_.Get(), 0);

	//ピクセルシェーダーでライト定数バッファの位置を設定
	buffnumber = 0;

	//更新された値でピクセルシェーダーのライト定数バッファを設定
	instanceptr_->getContext()->PSSetConstantBuffers(buffnumber, 1, lightbuffer_.GetAddressOf());

	return true;
}

void LightShader::renderShader(const int Indexcount)
{
	//頂点入力レイアウトを設定
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//この三角形のレンダリングに使用される頂点シェーダとピクセルシェーダを設定
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//サンプラー状態をピクセルシェーダーに設定
	instanceptr_->getContext()->PSSetSamplers(0, 1, samplerstate_.GetAddressOf());

	//三角形をレンダリング
	instanceptr_->getContext()->DrawIndexed(Indexcount, 0, 0);
}
