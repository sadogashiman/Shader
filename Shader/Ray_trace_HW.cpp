#include "stdafx.h"
#include "Ray_trace_HW.h"
#include"Direct3D.h"
#include"TextureFactory.h"

Ray_trace_HW::Ray_trace_HW()
{
}

Ray_trace_HW::~Ray_trace_HW()
{
}

bool Ray_trace_HW::init()
{
	HRESULT hr;

	support_.reset(new Support);
	if (!support_.get())
	{
		Error::showDialog("サポートクラスのメモリ確保に失敗");
		return false;
	}

	//各シェーダーデータの作成
	hr = support_.get()->createPixelData(L"raytraceps.cso");
	if (FAILED(hr))
	{
		Error::showDialog("ピクセルシェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createVertexData(L"raytracevs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("頂点シェーダーの作成に失敗");
		return false;
	}

	hr = support_.get()->createComputeData(L"raytracecs.cso");
	if (FAILED(hr))
	{
		Error::showDialog("コンピュートシェーダーの作成に失敗");
		return false;
	}

	//作成したデータを受け取る
	vertexshader_ = support_.get()->getVertexShader();
	pixelshader_ = support_.get()->getPixelShader();
	computeshader_ = support_.get()->getComputeShader();

	//頂点入力レイアウトの設定
	D3D11_INPUT_ELEMENT_DESC polygonlayout[2];
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

	unsigned int numelements = sizeof(polygonlayout) / sizeof(polygonlayout[0]);

	hr = support_.get()->createVertexInputLayout(polygonlayout, numelements);
	if (FAILED(hr))
	{
		Error::showDialog("頂点入力レイアウトの作成に失敗");
		return false;
	}

	//作成したデータを取得
	layout_ = support_.get()->getInputLayout();

	//データ作成
	StructuredElement* raydata;
	raydata = new StructuredElement[kWindow_Height * kWindow_Width]; //画面ピクセル分要素を取得
	Vector3 pixelpos(0.0F, 0.0F, 0.0F); //3D空間のピクセル座標
	Vector3 eyedir(0.0F, 0.0F, 1.0F);   //視線方向
	float dist = 0.0F; //交点までの長さ
	Vector3 dir(0.0F, 0.0F, 1.0F); //レイの方向

	for (int i = 0; i < kWindow_Height; i++)
	{
		for (int j = 0; j < kWindow_Width; j++)
		{
			pixelpos = transScreenToWorld(j, i);
			raydata[i * kWindow_Width + j].startpoint = eyepos_;
			raydata[i * kWindow_Width + j].direction = pixelpos - eyepos_;
		}
	}

	//データ渡し用のストラクチャードバッファを作成
	D3D11_BUFFER_DESC structuredbufferdesc;
	ZeroMemory(&structuredbufferdesc, sizeof(structuredbufferdesc));
	structuredbufferdesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	structuredbufferdesc.ByteWidth = sizeof(StructuredElement) * kWindow_Height * kWindow_Width;
	structuredbufferdesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	structuredbufferdesc.StructureByteStride = sizeof(StructuredElement);

	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = raydata;
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&structuredbufferdesc, &subresource, cpbufferinput_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//シェーダーリソースビュー(SRV)作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvdesc.BufferEx.FirstElement = 0;
	srvdesc.Format = DXGI_FORMAT_UNKNOWN;
	srvdesc.BufferEx.NumElements = kWindow_Height * kWindow_Width;

	hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(cpbufferinput_.Get(), &srvdesc, cpbufferinputsrv_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//データ受取用の構造体バッファ(ストラクチャードバッファ)を作成
	D3D11_BUFFER_DESC tex2ddesc;
	ZeroMemory(&tex2ddesc, sizeof(tex2ddesc));
	tex2ddesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	tex2ddesc.ByteWidth = sizeof(StructuredElement) * kWindow_Height * kWindow_Width;
	tex2ddesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	tex2ddesc.StructureByteStride = sizeof(StructuredElement);

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&tex2ddesc, NULL, cpbufferresult_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//UAV(Unordered Acces View)作成
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavdesc;
	ZeroMemory(&uavdesc, sizeof(uavdesc));
	uavdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavdesc.Buffer.FirstElement = 0;
	uavdesc.Format = DXGI_FORMAT_UNKNOWN;
	uavdesc.Buffer.NumElements = kWindow_Height * kWindow_Width;

	hr = Direct3D::getInstance()->getDevice()->CreateUnorderedAccessView(cpbufferresult_.Get(), &uavdesc, cpbufferresultuav_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//データ渡し用のコンスタントバッファを作成
	D3D11_BUFFER_DESC cbufferdesc;
	ZeroMemory(&cbufferdesc, sizeof(cbufferdesc));
	cbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbufferdesc.ByteWidth = sizeof(ConstantBufferType);
	cbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbufferdesc.MiscFlags = 0;
	cbufferdesc.Usage = D3D11_USAGE_DYNAMIC;

	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&cbufferdesc, NULL, cpconstantbuffer_.GetAddressOf());

	if (FAILED(hr))
	{
		return false;
	}

	//ダイナミックテクスチャの設定
	D3D11_TEXTURE2D_DESC dynamictexdesc;
	dynamictexdesc.Usage = D3D11_USAGE_DYNAMIC;
	dynamictexdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dynamictexdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	dynamictexdesc.MiscFlags = 0;
	dynamictexdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dynamictexdesc.MipLevels = 1;
	dynamictexdesc.ArraySize = 1;
	dynamictexdesc.SampleDesc.Count = 1;
	dynamictexdesc.SampleDesc.Quality = 0;
	dynamictexdesc.Width = kWindow_Width;
	dynamictexdesc.Height = kWindow_Height;

	//ダイナミックテクスチャの作成
	hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&dynamictexdesc, NULL, cpdynamictexture_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(cpdynamictexture_.Get(), NULL, &dynamictexture_);
	if (FAILED(hr))
	{
		return false;
	}

	//サンプラーを作成
	D3D11_SAMPLER_DESC sampledesc;
	ZeroMemory(&sampledesc, sizeof(D3D11_SAMPLER_DESC));
	sampledesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampledesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampledesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = Direct3D::getInstance()->getDevice()->CreateSamplerState(&sampledesc, samplerstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//球を生成
	Sphere sp;
	sp.position = Vector3::Zero;
	sp.r = 1;
	addSphere(&sp);


	return true;
}

bool Ray_trace_HW::render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Light* Light)
{
	bool result;

	result = setShaderParameters(World, View, Projection, Light);
	if (!result)
	{
		return false;
	}

	renderShader(Indexcount);

	return true;
}

void Ray_trace_HW::destroy()
{
	SAFE_RELEASE(vertexshader_);
	SAFE_RELEASE(pixelshader_);
	SAFE_RELEASE(computeshader_);
	SAFE_RELEASE(layout_);
	SAFE_RELEASE(dynamictexture_);
}

bool Ray_trace_HW::setShaderParameters(Matrix World, Matrix View, Matrix Projection, Light* Light)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedresource;
	ConstantBufferType* dataptr;


	//シェーダーの登録
	Direct3D::getInstance()->getContext()->VSSetShader(vertexshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->PSSetShader(pixelshader_, NULL, 0);
	Direct3D::getInstance()->getContext()->CSSetShader(computeshader_, NULL, 0);

	//コンピュートシェーダーにデータを渡す
	//バッファをロック
	hr = Direct3D::getInstance()->getContext()->Map(cpconstantbuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	dataptr = (ConstantBufferType*)mappedresource.pData;

	//データコピー
	dataptr->position = spheresrray_[0]->position;
	dataptr->r = spheresrray_[0]->r;

	//球のワールド逆行列
	spheresrray_[0]->im = XMMatrixTranslation(spheresrray_[0]->position.x, spheresrray_[0]->position.y, spheresrray_[0]->position.z);
	spheresrray_[0]->im = XMMatrixInverse(0, spheresrray_[0]->im);
	dataptr->sphereiw = spheresrray_[0]->im;
	dataptr->sphereiw = XMMatrixTranspose(dataptr->sphereiw);

	//ビューの逆行列
	dataptr->miw = XMMatrixInverse(0, View);
	dataptr->miw = XMMatrixTranspose(dataptr->miw);

	//ライト位置
	dataptr->lightposition = Light->getPosition();

	//ロック解除
	Direct3D::getInstance()->getContext()->Unmap(cpconstantbuffer_.Get(), 0);

	//このコンスタントバッファを使うシェーダーの登録
	Direct3D::getInstance()->getContext()->CSSetConstantBuffers(0, 1, cpconstantbuffer_.GetAddressOf());


	//コンスタントバッファをでレイトレースし結果をテクスチャに書きこむ
	//コンピュートシェーダー実行
	Direct3D::getInstance()->getContext()->CSSetShaderResources(0, 1, cpbufferinputsrv_.GetAddressOf());
	Direct3D::getInstance()->getContext()->CSSetUnorderedAccessViews(0, 1, cpbufferresultuav_.GetAddressOf(), 0);
	Direct3D::getInstance()->getContext()->Dispatch(kWindow_Width, kWindow_Height, 1);

	//コンピュートシェーダーの結果を受け取る
	ID3D11Buffer* copy = nullptr;
	StructuredElement* result = new StructuredElement[kWindow_Height * kWindow_Width];

	D3D11_BUFFER_DESC bufferdesc;
	cpbufferresult_.Get()->GetDesc(&bufferdesc);
	bufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferdesc.Usage = D3D11_USAGE_STAGING;
	bufferdesc.BindFlags = 0;
	bufferdesc.MiscFlags = 0;
	hr = Direct3D::getInstance()->getDevice()->CreateBuffer(&bufferdesc, NULL, &copy);
	if (FAILED(hr))
	{
		return false;
	}

	//コピー
	Direct3D::getInstance()->getContext()->CopyResource(copy, cpbufferresult_.Get());

	hr = Direct3D::getInstance()->getContext()->Map(copy, 0, D3D11_MAP_READ, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	memcpy(result, (StructuredElement*)mappedresource.pData, sizeof(StructuredElement) * kWindow_Height * kWindow_Width);
	
	//ロック解除
	Direct3D::getInstance()->getContext()->Unmap(copy, 0);

	//不要になったので破棄
	copy->Release();

	//テクスチャにデータを書き込む
	hr = Direct3D::getInstance()->getContext()->Map(cpdynamictexture_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedresource);
	if (FAILED(hr))
	{
		Error::showDialog("バッファのロックに失敗");
		return false;
	}

	//データ書き込み
	DWORD* texel = (DWORD*)mappedreso
		urce.pData;
	DWORD pitch = mappedresource.RowPitch;
	int cnt = 0;
	for (int i = 0; i < kWindow_Height; i++)
	{
		for (int j = 0; j < kWindow_Width; j++)
		{
			texel[j] = result[cnt].color;
			cnt++;
		}

		texel += pitch / (sizeof(DWORD) / sizeof(BYTE));
	}

	texel[0] = result[cnt].color;
	Direct3D::getInstance()->getContext()->Unmap(cpdynamictexture_.Get(), 0);

	//破棄
	delete[] result;


	return true;
}

void Ray_trace_HW::renderShader(const int IndexCount)
{
	//頂点入力レイアウトをセット
	Direct3D::getInstance()->getContext()->IASetInputLayout(layout_);

	//プリミティブモードを設定
	Direct3D::getInstance()->getContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//シェーダーに情報を渡す
	Direct3D::getInstance()->getContext()->PSSetSamplers(0, 1, samplerstate_.GetAddressOf());
	Direct3D::getInstance()->getContext()->PSSetShaderResources(0, 1, &dynamictexture_);

	Direct3D::getInstance()->getContext()->Draw(4, 0);
}

Vector3 Ray_trace_HW::transScreenToWorld(const int ScreenHeight, const int ScreenWidth)
{
	Vector3 screenpos;

	screenpos.z = 0.0F;
	screenpos.x = (static_cast<float>(ScreenWidth)) / static_cast<float>(kWindow_Width) * 2 - 1.0F;
	screenpos.y = -(static_cast<float>(ScreenHeight)) / static_cast<float>(kWindow_Height) * 2 - 1.0F;

	return screenpos;
}

void Ray_trace_HW::addSphere(Sphere* Sp)
{
	spheresrray_[numsphere_] = new Sphere;
	memcpy(spheresrray_[numsphere_], Sp, sizeof(Sphere));
	numsphere_++;
}
