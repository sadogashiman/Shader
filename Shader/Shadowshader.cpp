#include "stdafx.h"
#include "Shadowshader.h"
#include"Direct3D.h"


Shadowshader::Shadowshader()
{
	ZeroMemory(this, sizeof(Shadowshader));
}

Shadowshader::~Shadowshader()
{
}

bool Shadowshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[3];
	unsigned int numelements;
	D3D11_BUFFER_DESC lightbufferdesc;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"shadow_vs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"shadow_ps.cso",pixelshader_.GetAddressOf());
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

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements,layout_.GetAddressOf());
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

	//テクスチャのサンプラーを作成
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplestateclamp_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//サンプルタイプの違うものを作成
	samplerdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&samplerdesc, samplestatewrap_.GetAddressOf());
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

	//定数バッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
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

	//ライトバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, lightbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	lightbufferdesc.ByteWidth = sizeof(LightBufferType2);

	//二つ目のライトバッファを作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&lightbufferdesc, NULL, lightbuffer2_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool Shadowshader::render(const int Indexcound,Matrix World,Matrix View,Matrix Projection,Matrix lightview,Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 LightDirection, Vector4 Ambientcolor, Vector4 Diffusecolor)
{	bool result;

	//レンダリングに関するシェーダーパラメータを設定
	result = setShaderParameters(World, View, Projection, lightview, lightprojection, texture, Depthmaptexture, LightDirection, Ambientcolor, Diffusecolor);

	if (!result)
	{
		return false;
	}

	//準備したバッファをシェーダーでレンダリング
	renderShader(Indexcound);

	return true;
}

bool Shadowshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix Lightview, Matrix Lightprojection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 LightDirection, Vector4 Ambientcolor, Vector4 Diffusecolor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	LightBufferType* dataptr2;
	LightBufferType2* dataptr3;
	unsigned int buffnumber;

	//シェーダーように行列を転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);
	Lightview = XMMatrixTranspose(Lightview);
	Lightprojection = XMMatrixTranspose(Lightprojection);

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
	dataptr->lightview = Lightview;
	dataptr->lightprojection = Lightprojection;

	//定数バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//頂点シェーダで定数バッファの位置を設定
	buffnumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, matrixbuffer_.GetAddressOf());

	//シェーダーリソースを設定
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);
	Direct3D::getInstance()->getContext()->PSSetShaderResources(1, 1, &Depthmaptexture);

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD,0, & mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	dataptr2 = (LightBufferType*)mappedresouce.pData;

	//データ更新
	dataptr2->ambientColor = Ambientcolor;
	dataptr2->diffuseColor = Diffusecolor;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_.Get(), 0);

	buffnumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, lightbuffer_.GetAddressOf());

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer2_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//ポインタキャスト
	dataptr3 = (LightBufferType2*)mappedresouce.pData;

	dataptr3->lightposition = LightDirection;
	dataptr3->padding = 0.0F;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer2_.Get(), 0);

	buffnumber = 1;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, lightbuffer2_.GetAddressOf());

	return true;
}

void Shadowshader::renderShader(const int Indexcount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//レンダリングに使用するシェーダーを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//サンプラーの設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplestateclamp_.GetAddressOf());
	Direct3D::getInstance()->getContext()->PSSetSamplers(1, 1, samplestatewrap_.GetAddressOf());

	//レンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);
}
