#include "Ray_trace_HW.h"
#include"Direct3D.h"
#include"System.h"

Ray_trace_HW::Ray_trace_HW()
{
	ZeroMemory(this, sizeof(Ray_trace_HW));
	eyepos_ = Vector3(0.0F, 3.0F, -5.0F);
	lightpos_ = Vector3(-10.0F, 10.0F, -5.0F);
}

Ray_trace_HW::~Ray_trace_HW()
{
}

bool Ray_trace_HW::init()
{
	HRESULT hr;
	RECT rc;
	GetClientRect(System::getWindowHandle(), &rc);

	Sphere sp;
	sp.position = Vector3::Zero;
	sp.r = 1;
	addSphere(&sp);

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	hr = support_.get()->createComputeData(L"raytracecs.cso",computeshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("コンピュートシェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createVertexData(L"raytracevs.cso",vertexshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createPixelData(L"raytraceps.cso",pixelshader_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC polygonlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	support_.get()->createVertexInputLayout(polygonlayout, numelements,layout_.GetAddressOf());

	D3D11_TEXTURE2D_DESC dc;
	dc.Usage = D3D11_USAGE_DYNAMIC;
	dc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dc.MiscFlags = 0;
	dc.Width = System::getWindowWidth();
	dc.Height = System::getWindowHeight();
	dc.MipLevels = 1;
	dc.ArraySize = 1;
	dc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dc.SampleDesc.Count = 1;
	dc.SampleDesc.Quality = 0;

	hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&dc, NULL, cpdynamictexture_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("TEXTURE2Dの作成に失敗");
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(cpdynamictexture_.Get(), NULL, &dynamictexture_);
	if (FAILED(hr))
	{
		Error::showDialog("シェーダーリソースビューの作成に失敗");
		return false;
	}


	D3D11_SAMPLER_DESC sampledesc;
	ZeroMemory(&sampledesc, sizeof(sampledesc));
	sampledesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampledesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&sampledesc, cpsamplerstate_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("サンプラーの作成に失敗");
		return false;
	}

	VertexType vertices[] =
	{
		Vector3(-1.0F,-1.0F,0.0F),Vector2(0.0F,1.0F),
		Vector3(-1.0F,1.0F,0.0F),Vector2(0.0F,0.0F),
		Vector3(1.0F,-1.0F,0.0F),Vector2(1.0F,1.0F),
		Vector3(1.0F,1.0F,0.0F),Vector2(1.0F,0.0F),
	};

	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexType) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initdata;
	initdata.pSysMem = vertices;
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&bd, &initdata, vertexbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("頂点バッファの作成に失敗");
		return false;
	}

	SbufferIn* raydata = new SbufferIn[System::getWindowWidth() * System::getWindowHeight()];
	Vector3 pixelpos(0.0F, 0.0F, 0.0F);
	Vector3 eyedir(0.0F, 0.0F, 1.0F);
	float dist;
	Vector3 dir(0.0F, 0.0F, 1.0F);

	for(int sy = 0;sy<System::getWindowHeight();sy++)
	{
		for (int sx = 0; sx < System::getWindowWidth(); sx++)
		{
			pixelpos = transScreenToWorld(sx, sy);
			raydata[sy * System::getWindowWidth() + sx].E = eyepos_;
			raydata[sy * System::getWindowWidth() + sx].V = pixelpos - eyepos_;
		}
	}

	D3D11_BUFFER_DESC bc;
	ZeroMemory(&bc, sizeof(bc));
	bc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bc.ByteWidth = sizeof(SbufferIn) * System::getWindowHeight() * System::getWindowWidth();
	bc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bc.StructureByteStride = sizeof(SbufferIn);

	initdata.pSysMem = raydata;
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&bc, &initdata, cpbufferinput_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("バッファの作成に失敗");
		return false;
	}

	delete raydata;

	D3D11_SHADER_RESOURCE_VIEW_DESC srd;
	srd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srd.BufferEx.FirstElement = 0;
	srd.Format = DXGI_FORMAT_UNKNOWN;
	srd.BufferEx.NumElements = System::getWindowHeight() * System::getWindowWidth();
	
	hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(cpbufferinput_.Get(), &srd, cpbufferinputsrv_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("シェーダーリソースビューの作成に失敗");
		return false;
	}

	ZeroMemory(&bc, sizeof(bc));
	bc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bc.ByteWidth = sizeof(SbufferOut) * System::getWindowHeight() * System::getWindowWidth();
	bc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bc.StructureByteStride = sizeof(SbufferOut);

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&bc, NULL, cpbufferresult_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("バッファの作成に失敗");
		return false;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC ud;
	ZeroMemory(&ud, sizeof(ud));
	ud.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	ud.Buffer.FirstElement = 0;
	ud.Format = DXGI_FORMAT_UNKNOWN;
	ud.Buffer.NumElements = System::getWindowHeight() * System::getWindowWidth();

	hr = Direct3D::getInstance()->getDevice()->CreateUnorderedAccessView(cpbufferresult_.Get(), &ud, cpbufferresultuav_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("UAVの作成に失敗");
		return false;
	}

	ZeroMemory(&bc, sizeof(bc));
	bc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bc.ByteWidth = sizeof(ConstantBufferType);
	bc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bc.MiscFlags = 0;
	bc.Usage = D3D11_USAGE_DYNAMIC;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&bc, NULL, cpconstantbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		Error::showDialog("コンスタントバッファの作成に失敗");
		return false;
	}

	return true;
}

bool Ray_trace_HW::render()
{
	HRESULT hr;
	Matrix world;
	Matrix view;
	Matrix projection;

	//絶対値座標変換
	world = XMMatrixTranslation(0.0F, 0.0F, 0.0F);

	Vector3 lookat(0.0F, 0.0F, 0.0F);
	Vector3 upvec(0.0F, 1.0F, 0.0F);
	view = XMMatrixLookAtLH(eyepos_, lookat, upvec);

	projection = XMMatrixPerspectiveFovLH(XM_PI / 4, static_cast<float>(System::getWindowWidth()) / static_cast<float>(System::getWindowHeight()), 1, 100);

	//シェーダーの登録
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_.Get(), NULL, 0);
	Direct3D::getInstance()->getContext()->CSSetShader(computeshader_.Get(), NULL, 0);

	D3D11_MAPPED_SUBRESOURCE pdata;
	ConstantBufferType cb;

	hr = Direct3D::getInstance()->getContext()->Map(cpconstantbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}
	else
	{
		cb.position = spheresrray_[0]->position;
		cb.r = spheresrray_[0]->r;
		spheresrray_[0]->im = XMMatrixTranslation(spheresrray_[0]->position.x, spheresrray_[0]->position.y, spheresrray_[0]->position.z);
		spheresrray_[0]->im = XMMatrixInverse(0, spheresrray_[0]->im);
		cb.sphereiw = XMMatrixTranspose(cb.sphereiw);

		cb.miw = XMMatrixInverse(0, cb.miw);
		cb.miw = XMMatrixTranspose(cb.miw);

		cb.lightposition = lightpos_;

		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));
		Direct3D::getInstance()->getContext()->Unmap(cpconstantbuffer_.Get(), 0);

	}


	Direct3D::getInstance()->getContext()->CSSetConstantBuffers(0, 1, cpconstantbuffer_.GetAddressOf());;


	//コンピュートシェーダー実行
	Direct3D::getInstance()->getContext()->CSSetShaderResources(0, 1, cpbufferinputsrv_.GetAddressOf());
	Direct3D::getInstance()->getContext()->CSSetUnorderedAccessViews(0, 1, cpbufferresultuav_.GetAddressOf(), 0);
	Direct3D::getInstance()->getContext()->Dispatch(System::getWindowWidth(), System::getWindowHeight(), 1);

	//結果受け取り
	ID3D11Buffer* buffercopy = NULL;
	SbufferOut* presult = new SbufferOut[System::getWindowWidth() * System::getWindowHeight()];

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	cpbufferresult_.Get()->GetDesc(&bd);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bd.Usage = D3D11_USAGE_STAGING;
	bd.BindFlags = 0;
	bd.MiscFlags = 0;

	Direct3D::getInstance()->getDevice()->CreateBuffer(&bd, NULL, &buffercopy);
	Direct3D::getInstance()->getContext()->CopyResource(buffercopy, cpbufferresult_.Get());

	D3D11_MAPPED_SUBRESOURCE mappedresource;
	Direct3D::getInstance()->getContext()->Map(buffercopy, 0, D3D11_MAP_READ, 0, &mappedresource);
	memcpy(presult, (SbufferOut*)mappedresource.pData, sizeof(SbufferOut) * System::getWindowWidth() * System::getWindowHeight());
	Direct3D::getInstance()->getContext()->Unmap(buffercopy, 0);
	buffercopy->Release();

	hr = Direct3D::getInstance()->getContext()->Map(cpdynamictexture_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	DWORD* ptexel = (DWORD*)mappedresource.pData;
	DWORD pitch = mappedresource.RowPitch;
	int cnt = 0;
	int tmp = 0;
	for (int i = 0; i < System::getWindowHeight(); i++)
	{
		for (int k = 0; k < System::getWindowWidth(); k++)
		{
			ptexel[k] = presult[cnt].color;
			cnt++;
		}

		ptexel += pitch / (sizeof(DWORD) / sizeof(BYTE));
	}

	ptexel[0] = presult[cnt].color;
	Direct3D::getInstance()->getContext()->Unmap(cpdynamictexture_.Get(), 0);

	delete presult;


	//描画
	Direct3D::getInstance()->getContext()->ClearRenderTargetView(Direct3D::getInstance()->getRenderTargetView(), Colors::White);
	Direct3D::getInstance()->getContext()->ClearDepthStencilView(Direct3D::getInstance()->getStencilView(), D3D10_CLEAR_DEPTH, 1.0F, 0);

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	Direct3D::getInstance()->getContext()->IASetVertexBuffers(0, 1, vertexbuffer_.GetAddressOf(), &stride, &offset);
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_.Get());
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, cpsamplerstate_.GetAddressOf());
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &dynamictexture_);;

	Direct3D::getInstance()->getContext()->Draw(4, 0);
	Direct3D::getInstance()->getSwapChain()->Present(0, 0);

	return true;
}

bool Ray_trace_HW::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Light* Light)
{
	return true;
}

void Ray_trace_HW::renderShader(const int IndexCount)
{
}

Vector3 Ray_trace_HW::transScreenToWorld(const int ScreenHeight, const int ScreenWidth)
{
	Vector3 tmp;
	tmp.z = 0;
	tmp.x = (static_cast<float>(ScreenWidth) / static_cast<float>(System::getWindowWidth())) * 2 - 1.0F;
	tmp.y = -(static_cast<float>(ScreenHeight) / static_cast<float>(System::getWindowHeight())) * 2 - 1.0F;

	return tmp;
}

void Ray_trace_HW::addSphere(Sphere* Sp)
{
	spheresrray_[numsphere_] = new Sphere;
	memcpy(spheresrray_[numsphere_], Sp, sizeof(Sphere));
	numsphere_++;
}

