#include "stdafx.h"
#include "Colorshader.h"


Colorshader::Colorshader()
{
	instanceptr_ = Direct3D::getInstance();
}

Colorshader::~Colorshader()
{
}

bool Colorshader::init()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC colorbufferdesc;
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygonlayout;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	hr = support_.get()->createVertexData(L"Shader/color_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/color_ps.cso", pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//配列サイズ変更
	polygonlayout.resize(1);

	//頂点入力レイアウトの作成
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	//頂点入力レイアウトの作成
	hr = support_.get()->createVertexInputLayout(polygonlayout,layout_.GetAddressOf());
	if (FAILED(hr))
	{
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
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("定数バッファの作成に失敗");
		return false;
	}

	//カラーバッファを作成
	colorbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	colorbufferdesc.ByteWidth = sizeof(ColorBufferType);
	colorbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colorbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colorbufferdesc.MiscFlags = 0;
	colorbufferdesc.StructureByteStride = 0;

	//カラーバッファの作成
	hr = instanceptr_->getDevice()->CreateBuffer(&colorbufferdesc, NULL, colorbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("カラーバッファの作成に失敗");
	}

	return true;
}

bool Colorshader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, Vector4 Color)
{
	bool result;

	//シェーダーパラメーターの設定
	result = setShaderParameters(World, View, Projection,Color);
	if (!result)
	{
		Error::showDialog("シェーダーパラメーターの設定に失敗");
		return false;
	}

	//レンダリング
	renderShader(IndexCount);

	return true;
}

bool Colorshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 Color)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	ColorBufferType* dataptr2;
	unsigned int buffernumber;

	//シェーダーように行列を転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//バッファのロック
	hr = instanceptr_->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//定数バッファへのポインタを作成
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//データ更新
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロック解除
	instanceptr_->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//定数バッファをセット
	instanceptr_->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//ピクセルシェーダーの出力色を決定
	hr = instanceptr_->getContext()->Map(colorbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//ポインタキャスト
	dataptr2 = (ColorBufferType*)mappedresource.pData;

	//データ更新
	dataptr2->color = Color;

	//ロック解除
	instanceptr_->getContext()->Unmap(colorbuffer_.Get(), 0);

	//シェーダーにセット
	instanceptr_->getContext()->PSSetConstantBuffers(0, 1, colorbuffer_.GetAddressOf());


	return true;
}

void Colorshader::renderShader(const int IndexCount)
{
	//頂点入力レイアウトの作成
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//シェーダーセット
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//レンダリング
	instanceptr_->getContext()->DrawIndexed(IndexCount,0,0);
}