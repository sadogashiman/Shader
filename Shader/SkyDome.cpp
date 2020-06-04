#include "stdafx.h"
#include "SkyDome.h"
#include"Direct3D.h"

bool SkyDome::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	GradientBufferType* dataptr2;
	unsigned int buffernumber;

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

	//ロックしたバッファのポインタを取得
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//定数バッファにコピー
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//バッファのロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//定数バッファをセット
	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//グラデーション用バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(gradientbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//ロックしたバッファのポインタを取得
	dataptr2 = (GradientBufferType*)mappedresource.pData;

	//定数バッファにコピー
	dataptr2->apexcolor = ApexColor;
	dataptr2->centorcolor = CentorColor;

	//ロックを解除
	Direct3D::getInstance()->getContext()->Unmap(gradientbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//グラデーション用バッファをセット
	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, gradientbuffer_.GetAddressOf());

	return true;
}

void SkyDome::renderShader(const int IndexCount)
{
	//頂点入力レイアウトをセット
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());

	//シェーダーをセット
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//レンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);

}

bool SkyDome::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC gradientbufferdesc;

	//サポートクラスを生成
	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"skydomevs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"skydomeps.cso", pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	//頂点入力レイアウトを作成
	polygonlayout[0].SemanticName = "POSITION";
	polygonlayout[0].SemanticIndex = 0;
	polygonlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonlayout[0].InputSlot = 0;
	polygonlayout[0].AlignedByteOffset = 0;
	polygonlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[0].InstanceDataStepRate = 0;

	numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements, layout_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
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
		return false;
	}

	//グラデーション用バッファの設定
	gradientbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientbufferdesc.ByteWidth = sizeof(GradientBufferType);
	gradientbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientbufferdesc.MiscFlags = 0;
	gradientbufferdesc.StructureByteStride = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&gradientbufferdesc, NULL, gradientbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("グラデーション用バッファの作成に失敗");
		return false;
	}

	return true;
}

bool SkyDome::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	bool result;

	//パラメーターをセット
	result = setShaderParameters(World,View,Projection,ApexColor,CentorColor);
	if (!result)
	{
		Error::showDialog("シェーダーパラメーターの設定に失敗");
		return false;
	}

	renderShader(Indexcount);

	return true;
}
