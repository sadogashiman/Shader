#include "stdafx.h"
#include "Depthshader.h"

Depthshader::Depthshader()
{
	ZeroMemory(this, sizeof(Depthshader));
	instanceptr_ = Direct3D::getInstance();
}

Depthshader::~Depthshader()
{
}

bool Depthshader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];
	D3D11_BUFFER_DESC matrixbufferdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"Shader/depth_vs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/depth_ps.cso",pixelshader_.GetAddressOf());
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

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, _countof(polygonlayout),layout_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
		return false;
	}

	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//バッファ作成
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
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
	buffernumber = 0;

	//更新された値で頂点シェーダの定数バッファを最後に設定
	instanceptr_->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	return true;
}

void Depthshader::renderShader(int Indexcount)
{
	//頂点入力レイアウトを設定
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//レンダリングに使用するシェーダーを設定
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//レンダリング
	instanceptr_->getContext()->DrawIndexed(Indexcount, 0, 0);

}
