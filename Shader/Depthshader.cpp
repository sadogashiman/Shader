#include "stdafx.h"
#include "Depthshader.h"
#include"Direct3D.h"
#include"checkinputlayout.h"

Depthshader::Depthshader()
{
	vertexshader_ = nullptr;
	pixelshader_ = nullptr;
	layout_ = nullptr;
	matrixbuffer_ = nullptr;

}

Depthshader::~Depthshader()
{
}

bool Depthshader::init()
{
	bool result;
	result = initShader(L"depthvs.hlsl", L"depthps.hlsl");
	if (!result)
	{
		return false;
	}

	return true;
}

void Depthshader::destroy()
{
	destroyShader();
}

bool Depthshader::render(int indexCount, Matrix World, Matrix View, Matrix Projection)
{
	bool result;
	result = setShaderParameters(World, View, Projection);
	if (!result)
	{
		return false;
	}

	renderShader(indexCount);

	return true;
}

bool Depthshader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	HRESULT hr;
	ID3D10Blob* vertexshaderbuffer;
	ID3D10Blob* pixelshaderbuffer;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;

	pixelshaderbuffer = nullptr;
	vertexshaderbuffer = nullptr;

	hr = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr = D3DCompileFromFile(vsFilename, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexshaderbuffer, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreatePixelShader(pixelshaderbuffer->GetBufferPointer(), pixelshaderbuffer->GetBufferSize(), nullptr, &pixelshader_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreateVertexShader(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), nullptr, &vertexshader_);
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

	//レイアウト内の要素の数を取得
	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

#ifdef _DEBUG
	if (!checkInputLayout(vertexshaderbuffer->GetBufferPointer(), vertexshaderbuffer->GetBufferSize(), polygonlayout, numelements))
	{
		return false;
	}
#endif //_DEBUG

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

	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//バッファ作成
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, &matrixbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Depthshader::destroyShader()
{
	SAFE_RELEASE(matrixbuffer_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(vertexshader_);
}

bool Depthshader::setShaderParameters( Matrix World, Matrix View, Matrix Projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresouce;
	MatrixBufferType* dataptr;
	unsigned int buffernumber;

	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

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

	//定数バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//頂点シェーダで定数バッファの位置を設定
	buffernumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, &matrixbuffer_);

	return true;
}

void Depthshader::renderShader(int Indexcount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//レンダリングに使用するシェーダーを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//レンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(Indexcount, 0, 0);

}
