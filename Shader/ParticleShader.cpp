#include "stdafx.h"
#include "ParticleShader.h"

ParticleShader::ParticleShader()
{
	ZeroMemory(this, sizeof(ParticleShader));
	instanceptr_ = Direct3D::getInstance();
}

ParticleShader::~ParticleShader()
{
}

bool ParticleShader::init()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC polygonlayout[3];
	D3D11_BUFFER_DESC matrixbufferdesc;
	D3D11_SAMPLER_DESC samplerdesc;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//シェーダーコンパイル
	hr = support_.get()->createVertexData(L"Shader/particle_vs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = support_.get()->createPixelData(L"Shader/particle_ps.cso",pixelshader_.GetAddressOf());
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

	polygonlayout[2].SemanticName = "COLOR";
	polygonlayout[2].SemanticIndex = 0;
	polygonlayout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonlayout[2].InputSlot = 0;
	polygonlayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonlayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonlayout[2].InstanceDataStepRate = 0;

	//頂点入力レイアウトを作成
	hr = support_.get()->createVertexInputLayout(polygonlayout, _countof(polygonlayout),layout_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//動的定数バッファの設定
	matrixbufferdesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixbufferdesc.ByteWidth = sizeof(MatrixBufferType);
	matrixbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixbufferdesc.MiscFlags = 0;
	matrixbufferdesc.StructureByteStride = 0;

	//定数バッファのポインターを作成
	hr = instanceptr_->getDevice()->CreateBuffer(&matrixbufferdesc, NULL, matrixbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("定数バッファの作成に失敗");
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

	//サンプラーを作成
	hr = instanceptr_->getDevice()->CreateSamplerState(&samplerdesc, samplerstate_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("サンプラーの作成に失敗");
		return false;
	}

	return true;
}

bool ParticleShader::render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	bool result;

	//シェーダーのパラメーターを設定
	result = setShaderParameters(World, View, Projection, Texture);
	if (!result)
	{
		return false;
	}

	//準備されたバッファをシェーダーでレンダリング
	renderrShader(IndexCount);

	return true;
}

void ParticleShader::destroy()
{
}

void ParticleShader::renderrShader(const int IndexCount)
{
	//頂点入力レイアウトを設定
	instanceptr_->getContext()->IASetInputLayout(layout_.Get());

	//シェーダーを設定
	instanceptr_->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	instanceptr_->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);

	//サンプラーをピクセルシェーダーに適応
	instanceptr_->getContext()->PSSetSamplers(0, 1, samplerstate_.GetAddressOf());

	//レンダリング
	instanceptr_->getContext()->DrawIndexed(IndexCount,0,0);

}

bool ParticleShader::setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	MatrixBufferType* dataptr;
	unsigned int buffernumber;

	//行列をシェーダーように準備
	World = XMMatrixTranspose(World);
	View = XMMatrixTranspose(View);
	Projection = XMMatrixTranspose(Projection);

	hr = instanceptr_->getContext()->Map(matrixbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("シェーダーのロックに失敗");
		return false;
	}

	dataptr = (MatrixBufferType*)mappedresource.pData;

	dataptr->world = World;
	dataptr->view = View;
	dataptr->projection = Projection;

	//ロック解除
	instanceptr_->getContext()->Unmap(matrixbuffer_.Get(), 0);

	//定数バッファの位置を設定
	buffernumber = 0;

	//定数バッファを設定
	instanceptr_->getContext()->VSSetConstantBuffers(buffernumber, 1, matrixbuffer_.GetAddressOf());

	//テクスチャリソースを設定
	instanceptr_->getContext()->PSSetShaderResources(0, 1, &Texture);

	return true;
}