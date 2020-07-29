#include "stdafx.h"
#include "SpotlightShader.h"


SpotlightShader::SpotlightShader()
{
	ZeroMemory(this, sizeof(SpotlightShader));
	instanceptr_ = Direct3D::getInstance();
}

SpotlightShader::~SpotlightShader()
{
}

bool SpotlightShader::init()
{
	HRESULT hr;
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygonlayout;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	//サポートクラス生成
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	hr = support_.get()->createVertexData(L"Shader/spotlight_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/spotlight_ps.cso", pixelshader_.GetAddressOf());
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

	//サンプラーを設定
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

	hr = instanceptr_->getDevice()->CreateSamplerState(&samplerdesc, sampler_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//マトリックスバッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//マトリックスバッファを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc,NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ライトバッファの設定
	lightbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	lightbufferdesc.ByteWidth = sizeof(LightBufferType);
	lightbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightbufferdesc.MiscFlags = 0;
	lightbufferdesc.StructureByteStride = 0;

	//ライトバッファの作成
	hr = instanceptr_->getDevice()->CreateBuffer(&lightbufferdesc, NULL,lightbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool SpotlightShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector3 LightPosition, float Range, Vector4 DiffuseColor, Vector4 AmbientColor, Vector3 LightDirection, float Cone, Vector3 Att)
{
	bool result;

	//レンダリングに関するパラメーターを設定
	result = setShaderParameters(World, View, Projection,Texture, LightPosition, Range, DiffuseColor, AmbientColor, LightDirection, Cone, Att);
	if (!result)
	{
		return false;
	}

	//レンダリング
	renderShader(IndexCount);

	return true;
}

bool SpotlightShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector3 LightPosition, float Range, Vector4 DiffuseColor, Vector4 AmbientColor, Vector3 LightDirection, float Cone, Vector3 Att)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	unsigned int buffernumber;

	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//バッファをロック
	hr = instanceptr_->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		return false;
	}

	//ポインタキャスト
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//データコピー
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロック解除
	instanceptr_->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//データ更新
	instanceptr_->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());
	
	//テクスチャセット
	instanceptr_->getContext()->PSSetShaderResources(0, 1, &Texture);

	//ライトバッファロック
	hr = instanceptr_->getContext()->Map(lightbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		return false;
	}

	//ポインタキャスト
	dataptr2 = (LightBufferType*)mappedresource.pData;

	dataptr2->ambientcolor = AmbientColor;
	dataptr2->attenuator_ = Att;
	dataptr2->cone = Cone;
	dataptr2->diffusecolor = DiffuseColor;
	dataptr2->direction = LightDirection;
	dataptr2->padding = 0.0F;
	dataptr2->position = LightPosition;
	dataptr2->range = Range;

	//ロック解除
	instanceptr_->getContext()->Unmap(lightbuffer_.Get(), 0);

	//スロット番号
	buffernumber = 0;

	instanceptr_->getContext()->PSSetConstantBuffers(buffernumber, 1, lightbuffer_.GetAddressOf());

	return true;
}

void SpotlightShader::renderShader(const int IndexCount)
{
	//頂点入力レイアウトをセット
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//シェーダーセット
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//サンプラーセット
	instanceptr_->getContext()->PSSetSamplers(0, 1, sampler_.GetAddressOf());

	//レンダリング
	instanceptr_->getContext()->DrawIndexed(IndexCount, 0, 0);

}
