#include "stdafx.h"
#include "Shadowshader.h"
#include"Direct3D.h"


Shadowshader::Shadowshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	samplestateclamp_ = nullptr;
	samplestatewrap_ = nullptr;
	matrixbuffer_ = nullptr;
	lightbuffer_ = nullptr;
	lightbuffer2_ = nullptr;
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
	D3D11_BUFFER_DESC lightbufferdesc2;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"shadowvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"shadowps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	//コンパイル済みシェーダーを取得
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

	polygonlayout[2].SemanticName = "NORMAL";
	polygonlayout[2].SemanticIndex = 0;
	polygonlayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[2].InputSlot = 0;
	polygonlayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[2].InstanceDataStepRate = 0;

	//レイアウト内の要素の数を取得
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

#ifdef _DEBUG
	//データが有効か確認
	if (!Support::checkInputLayoutData(support_.get()->getVertexBufferPtr(), support_.get()->getVertexBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif // _DEBUG
	//頂点入力レイアウトの作成
	hr = Direct3D::getInstance()->getDevice()->CreateInputLayout(polygonlayout, numelements, support_.get()->getVertexBufferPtr(), support_.get()->getVertexBufferSize(), &layout_);
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

void Shadowshader::destroy()
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

bool Shadowshader::render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, const Matrix lightview, const Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor)
{	bool result;

	//レンダリングに関するシェーダーパラメータを設定
	result = setShaderParameters(World, View, Projection, lightview, lightprojection, texture, Depthmaptexture, lightposition, Ambientcolor, Diffusecolor);

	if (!result)
	{
		return false;
	}

	//準備したバッファをシェーダーでレンダリング
	renderShader(Indexcound);

	return true;
}

bool Shadowshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix Lightview, Matrix Lightprojection, ID3D11ShaderResourceView* Texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 Lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor)
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

	//定数バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//頂点シェーダで定数バッファの位置を設定
	buffnumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffnumber, 1, &matrixbuffer_);

	//シェーダーリソースを設定
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &Texture);
	Direct3D::getInstance()->getContext()->PSGetShaderResources(1, 1, &Depthmaptexture);

	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer_, 0, D3D11_MAP_WRITE_DISCARD,0, & mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	dataptr2 = (LightBufferType*)mappedresouce.pData;

	dataptr2->AmbientColor = Ambientcolor;
	dataptr2->DiffuseColor = Diffusecolor;

	Direct3D::getInstance()->getContext()->Unmap(lightbuffer_, 0);

	buffnumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffnumber, 1, &lightbuffer_);

	//2番目のライト定数バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(lightbuffer2_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresouce);
	if (FAILED(hr))
	{
		return false;
	}

	//シェーダー内のデータを更新
	dataptr3 = (LightBufferType2*)mappedresouce.pData;
	dataptr3->lightposition = Lightposition;
	dataptr3->padding1 = 0.0F;

	//ロックを解除
	Direct3D::getInstance()->getContext()->Unmap(lightbuffer2_, 0);

	//バッファ番号を設定
	buffnumber = 1;

	//頂点シェーダーに定数バッファをセット
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
