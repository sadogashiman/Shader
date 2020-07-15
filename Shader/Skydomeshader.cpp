#include "stdafx.h"
#include "Skydomeshader.h"

Skydomeshader::Skydomeshader()
{
	ZeroMemory(this, sizeof(Skydomeshader));
	instanceptr_ = Direct3D::getInstance();
}

Skydomeshader::~Skydomeshader()
{

}

bool Skydomeshader::init()
{
	HRESULT hr;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_BUFFER_DESC gradientbufferdesc;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[1];

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダーを読み込み
	hr = support_.get()->createVertexData(L"Shader/skydome_vs.cso", vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/skydome_ps.cso", pixelshader_.GetAddressOf());
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

	//頂点入力レイアウトの作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, _countof(polygonlayout), layout_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
		return false;
	}

	//マトリックスバッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//マトリックスバッファの作成
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("マトリックスバッファの作成に失敗");
		return false;
	}

	//グラデーション用バッファの設定
	gradientbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	gradientbufferdesc.ByteWidth = sizeof(GradientBufferType);
	gradientbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gradientbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gradientbufferdesc.MiscFlags = 0;
	gradientbufferdesc.StructureByteStride = 0;

	//グラデーション用バッファの作成
	hr = instanceptr_->getDevice()->CreateBuffer(&gradientbufferdesc, NULL, gradientbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("グラデーション用バッファの作成に失敗");
		return false;
	}

	return true;
}

bool Skydomeshader::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	bool result;

	result = setShaderParameters(World, View, Projection, ApexColor, CentorColor);
	if (!result)
	{
		Error::showDialog("シェーダーパラメーターの設定に失敗");
		return false;
	}

	//レンダリング
	renderShader(Indexcount);

	return true;
}

bool Skydomeshader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	GradientBufferType* dataptr2;
	unsigned int buffernumber;

	//行列をシェーダー用に転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//バッファロック
	hr = instanceptr_->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//シェーダー内のデータへのポインタを作成
	dataptr = (MatrixBufferType*)mappedresource.pData;

	//データを更新
	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロック解除
	instanceptr_->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//定数バッファをセット
	instanceptr_->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//バッファロック
	hr = instanceptr_->getContext()->Map(gradientbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//シェーダー内のデータへのポインタを作成
	dataptr2 = (GradientBufferType*)mappedresource.pData;

	//データを更新
	dataptr2->apexcolor = ApexColor;
	dataptr2->centorcolor = CentorColor;

	//ロック解除
	instanceptr_->getContext()->Unmap(gradientbuffer_.Get(), 0);

	//スロット番号を設定
	buffernumber = 0;

	//更新した値でピクセルシェーダーの定数バッファを更新
	instanceptr_->getContext()->PSSetConstantBuffers(buffernumber, 1, gradientbuffer_.GetAddressOf());

	return true;
}

void Skydomeshader::renderShader(const int IndexCount)
{
	//頂点入力レイアウトをセット
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//シェーダーをセット
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(),NULL, 0);
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);

	//レンダリング
	instanceptr_->getContext()->DrawIndexed(IndexCount, 0, 0);
}