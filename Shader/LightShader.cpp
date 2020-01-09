#include "stdafx.h"
#include "Lightshader.h"
#include"Direct3D.h"
#include"release.h"


LightShader::LightShader()
{
	vertexshader_ = 0;
	pixelshader_ = 0;
	layout_ = 0;
	samplerstate_ = 0;
	matrixbuff_ = 0;
	lightbuffer_ = 0;
}

LightShader::~LightShader()
{
}

bool LightShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	//Supportクラス生成
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("Supportクラスの生成に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"lightvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"lightps.cso");
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

#ifdef _DEBUG
	//データが有効か確認
	if (!Support::checkInputLayout(support_->getPixelBufferPtr(), support_->getPixelBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif // _DEBUG
	//頂点入力レイアウトの作成
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, support_->getPixelBufferPtr(), support_->getPixelBufferSize(), &layout_);
	if (FAILED(hr))
	{
		return false;
	}

	//不要になったデータの削除
	support_.get()->destroyBufferData();

	//動的マトリックス定数バッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//このクラスから頂点シェーダの定数バッファにアクセスできるようにポインタを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuff_);
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
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, &samplerstate_);
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
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, &lightbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void LightShader::destroy()
{
	//各オブジェクトの破棄
	SAFE_RELEASE(lightbuffer_);
	SAFE_RELEASE(matrixbuff_);
	SAFE_RELEASE(samplerstate_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool LightShader::render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* NormalTexture, Vector3 LightDirection)
{
	bool result;

	//レンダリングに関するシェーダーパラメータを設定
	result = SetShaderParameters(World, View, Projection,texture,NormalTexture,LightDirection);

	if (!result)
	{
		return false;
	}

	//準備したバッファをシェーダーでレンダリング
	renderShader(Indexcound);

	return true;
}

bool LightShader::SetShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* NormalTexture, Vector3 LightDirection)
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
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuff_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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
	Direct3D::getInstance()->getContext()->Unmap(matrixbuff_, 0);

	//頂点シェーダで定数バッファの位置を設定
	buffnumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuff_);

	//ピクセルシェーダーでテクスチャリソースを設定
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &texture);
	Direct3D::getInstance()->getContext()->PSSetShaderResources(1, 1, &NormalTexture);	//ライト定数バッファをロックして書き込み可能に

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
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
	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_, 0);

	//ピクセルシェーダーでライト定数バッファの位置を設定
	buffnumber = 0;

	//更新された値でピクセルシェーダーのライト定数バッファを設定
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, &lightbuffer_);

	return true;
}

void LightShader::renderShader(const int Indexcount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);
	//この三角形のレンダリングに使用される頂点シェーダとピクセルシェーダを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//サンプラー状態をピクセルシェーダーに設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplerstate_);

	//三角形をレンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
