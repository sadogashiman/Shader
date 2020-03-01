#include "stdafx.h"
#include "FontShader.h"
#include"Direct3D.h"

FontShader::FontShader()
{
}

FontShader::~FontShader()
{
}

bool FontShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
	unsigned int numelements;
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダー読み込み
	hr = support_.get()->createVertexData(L"fontvs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"fontps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	//作成されたデータをコピー
	vertexshader_ = support_.get()->getVertexShader();
	pixelshader_ = support_.get()->getPixelShader();


	
	return true;
}

bool FontShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color)
{
	bool result;

	//シェーダーのパラメーターを設定
	result = setShaderParameters(World, View, Projection, Texture, Color);
	if (!result)
	{
		Error::showDialog("シェーダーパラメーターの設定に失敗");
		return false;
	}

	renderShader(IndexCount);

	return true;
}

bool FontShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	PixelBufferType* dataptr2;
	unsigned int buffernumber;

	//シェーダーように行列を転置
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	//バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(matrixbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//バッファへのポインタを作成
	dataptr = (MatrixBufferType*)mappedresource.pData;

	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロックを解除
	Direct3D::getInstance()->getContext()->Unmap(matrixbuffer_, 0);

	//バッファをセットする場所を設定
	buffernumber = 0;

	Direct3D::getInstance()->getContext()->VSSetConstantBuffers(buffernumber, 1, &matrixbuffer_);

	//テクスチャをシェーダーにセット
	Direct3D::getInstance()->getContext()->PSGetShaderResources(0, 1, &Texture);

	hr = Direct3D::getInstance()->getContext()->Map(pixelbuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//バッファへのポインタを作成
	dataptr2 = (PixelBufferType*)mappedresource.pData;

	dataptr2->pixelcolor = Color;

	//ロック解除
	Direct3D::getInstance()->getContext()->Unmap(pixelbuffer_, 0);

	buffernumber = 0;

	Direct3D::getInstance()->getContext()->PSSetConstantBuffers(buffernumber, 1, &pixelbuffer_);

	return true;
}

void FontShader::renderShader(const int IndexCount)
{
	//頂点入力レイアウトを設定
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//レンダリングするシェーダーを設定
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);

	//ピクセルシェーダーにサンプラーを設定
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, &samplestate_);

	//フォントデータをレンダリング
	Direct3D::getInstance()->getContext()->DrawIndexed(IndexCount, 0, 0);
}