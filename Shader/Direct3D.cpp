#include "stdafx.h"
#include "Direct3D.h"


Direct3D::Direct3D()
{
	ZeroMemory(this, sizeof(Direct3D));
}

Direct3D::~Direct3D()
{
}


bool Direct3D::init(const int ScreenWidth, const int ScreenHeight, const bool Vsync, HWND Hwnd, const bool FullScreen, const float ScreenDepth, const float ScreenNear)
{
	HRESULT hr;
	IDXGIFactory* factry;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapteroutput;
	size_t stringlength;
	DXGI_MODE_DESC* displaymodelist;
	DXGI_ADAPTER_DESC adapterdesc;
	DXGI_SWAP_CHAIN_DESC swapchaindesc;
	D3D_FEATURE_LEVEL featurelebel[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	ID3D11Texture2D* backbufferptr;
	D3D11_TEXTURE2D_DESC depthbufferdesc;
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	D3D11_DEPTH_STENCIL_DESC depthdisabledstencildesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthstencilviewdesc;
	D3D11_RASTERIZER_DESC rasterdesc;
	D3D11_BLEND_DESC blendstatedesc;
	unsigned int nummodes = 0;
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	int error;
	float fieldofview;
	float screenaspect;

	//垂直同期設定
	vsync_ = Vsync;

	//DirectXグラフィックスインターフェイスを作成
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factry);
	if (FAILED(hr))
	{
		return false;
	}

	//ファクトリを使用してプライマリグラフィックスインターフェイス(ビデオカード)用のアダプターを作成
	hr = factry->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		return false;
	}

	//プライマリアダプターの出力(モニター)を列挙
	hr = adapter->EnumOutputs(0, &adapteroutput);
	if (FAILED(hr))
	{
		return false;
	}

	//アダプター出力の適合モードの数を取得
	hr = adapteroutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nummodes, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	//モニターとビデオカードの組み合わせで可能なすべての表示モードを保持するリストを作成
	//ビデオカードが複数ある場合最高設定できるものを使用
	displaymodelist = new DXGI_MODE_DESC[nummodes];
	if (!displaymodelist)
	{
		return false;
	}

	//表示モードのリストを作成
	hr = adapteroutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nummodes, displaymodelist);
	if (FAILED(hr))
	{
		return false;
	}

	//見つかった表示モードの中から画面幅と高さに一致するものを見つける
	for (unsigned int i = 0; i < nummodes; i++)
	{
		if (displaymodelist[i].Width == static_cast<unsigned int>(ScreenWidth))
		{
			if (displaymodelist[i].Height == static_cast<unsigned int>(ScreenHeight))
			{
				numerator = displaymodelist[i].RefreshRate.Numerator;
				denominator = displaymodelist[i].RefreshRate.Denominator;
			}
		}
	}

	//アダプタを取得
	hr = adapter->GetDesc(&adapterdesc);
	if (FAILED(hr))
	{
		return false;
	}

	//ビデオカードのメモリをメガバイト単位で確保
	videocardmemory_ = static_cast<int>(adapterdesc.DedicatedVideoMemory / 1024 / 1024);

	//ビデオカードの名前を文字列に変換して保存
	error = wcstombs_s(&stringlength, videocarddescription_, 128, adapterdesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//表示モードのリストを解放
	delete[] displaymodelist;
	displaymodelist = nullptr;

	//アダプター出力を解放
	adapter->Release();
	adapter = nullptr;

	//ファクトリーを解放
	factry->Release();
	factry = nullptr;


	//****************************************
	//        DirectXの初期化
	//****************************************

	//スワップチェーンの初期化
	ZeroMemory(&swapchaindesc, sizeof(swapchaindesc));

	//バックバッファの設定
	swapchaindesc.BufferCount = 1;

	//バックバッファの幅と高さを設定
	swapchaindesc.BufferDesc.Width = ScreenWidth;
	swapchaindesc.BufferDesc.Height = ScreenHeight;

	//バックバッファに32ビットサーフェイスを設定
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//バックバッファのリフレッシュレートを設定
	if (vsync_)
	{
		swapchaindesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapchaindesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapchaindesc.BufferDesc.RefreshRate.Numerator = 0;
		swapchaindesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//バックバッファの使用方法を設定
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//レンダリングするウィンドウの使用方法を設定
	swapchaindesc.OutputWindow = Hwnd;

	//マルチサンプリングの設定
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.SampleDesc.Quality = 0;

	//ウィンドウモードの設定
	if (FullScreen)
	{
		swapchaindesc.Windowed = false;
	}
	else
	{
		swapchaindesc.Windowed = true;
	}

	//スキャンラインの順序とスケーリングの設定
	swapchaindesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchaindesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//表示した後バックバッファを破棄するよう設定
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//アドバンスフラグの設定
	swapchaindesc.Flags = 0;

	//スワップチェーン・Direct3Dデバイス・Direct3Dデバイスコンテキストの作成

	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		featurelebel,
		3,
		D3D11_SDK_VERSION,
		&swapchaindesc,
		cpswapchain_.GetAddressOf(),
		cpdevice_.GetAddressOf(),
		NULL,
		cpdevicecontext_.GetAddressOf());
	if (FAILED(hr))
	{
		//設定したどの機能レベルでも設定ができない場合代わりにCPUを使用する(処理速度が低下する)
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			D3D_DRIVER_TYPE_WARP,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			featurelebel,
			3,
			D3D11_SDK_VERSION,
			&swapchaindesc,
			cpswapchain_.GetAddressOf(),
			cpdevice_.GetAddressOf(),
			NULL,
			cpdevicecontext_.GetAddressOf());
		if (FAILED(hr))
		{
			//それでも設定できない場合エラーとして処理する
			return false;
		}
	}

	//バックバッファへのポインタを取得
	hr = cpswapchain_.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbufferptr);
	if (FAILED(hr))
	{
		return false;
	}

	//バックバッファのポインタでレンダーターゲットビューを作成
	hr = cpdevice_.Get()->CreateRenderTargetView(backbufferptr, NULL, cprendertarget_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//不要になったバックバッファへのポインタを解放
	backbufferptr->Release();
	backbufferptr = nullptr;


	//深度バッファの初期化
	ZeroMemory(&depthbufferdesc, sizeof(depthbufferdesc));

	depthbufferdesc.Width = ScreenWidth;
	depthbufferdesc.Height = ScreenHeight;
	depthbufferdesc.MipLevels = 1;
	depthbufferdesc.ArraySize = 1;
	depthbufferdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthbufferdesc.SampleDesc.Count = 1;
	depthbufferdesc.SampleDesc.Quality = 0;
	depthbufferdesc.Usage = D3D11_USAGE_DEFAULT;
	depthbufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthbufferdesc.CPUAccessFlags = 0;
	depthbufferdesc.MiscFlags = 0;

	//設定した内容を使って深度バッファのテクスチャを作成
	hr = cpdevice_.Get()->CreateTexture2D(&depthbufferdesc, NULL, cpdepthstencilbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ステンシルの初期化
	ZeroMemory(&depthstencildesc, sizeof(depthstencildesc));

	//ステンシルの設定
	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthstencildesc.StencilEnable = true;
	depthstencildesc.StencilReadMask = 0xFF;
	depthstencildesc.StencilWriteMask = 0xFF;

	//ピクセルが前を向いている時のステンシル操作
	depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//ピクセルが後ろを向いている時のステンシル操作
	depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//深度ステンシルの操作
	hr = cpdevice_.Get()->CreateDepthStencilState(&depthstencildesc, cpdepthstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//深度ステンシルの状態を設定
	cpdevicecontext_.Get()->OMSetDepthStencilState(cpdepthstate_.Get(), 1);

	//弐番目のステンシルを初期化
	ZeroMemory(&depthdisabledstencildesc, sizeof(depthdisabledstencildesc));

	//ステンシルの設定
	depthdisabledstencildesc.DepthEnable = true;
	depthdisabledstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthdisabledstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthdisabledstencildesc.StencilEnable = true;
	depthdisabledstencildesc.StencilReadMask = 0xFF;
	depthdisabledstencildesc.StencilWriteMask = 0xFF;

	//ピクセルが前を向いている時のステンシル操作
	depthdisabledstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthdisabledstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//ピクセルが後ろを向いている時のステンシル操作
	depthdisabledstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthdisabledstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = cpdevice_.Get()->CreateDepthStencilState(&depthdisabledstencildesc, cpdepthdisabledstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ブレンドの状態をクリア
	ZeroMemory(&blendstatedesc, sizeof(D3D11_BLEND_DESC));

	//設定を変更してアルファを無効化
	blendstatedesc.RenderTarget[0].BlendEnable = FALSE;

	//設定を使用してブレンドステートを作成
	hr = cpdevice_.Get()->CreateBlendState(&blendstatedesc, cpenabledalphablendstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//深度ステンシルビューの初期化
	ZeroMemory(&depthstencilviewdesc, sizeof(depthstencilviewdesc));

	depthstencilviewdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthstencilviewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthstencilviewdesc.Texture2D.MipSlice = 0;

	//深度ステンシルビューの作成
	hr = cpdevice_.Get()->CreateDepthStencilView(cpdepthstencilbuffer_.Get(), &depthstencilviewdesc, cpdepthview_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	//レンダーターゲットと深度ステンシルバッファを出力レンダーパイプラインにバインド
	cpdevicecontext_.Get()->OMSetRenderTargets(1, cprendertarget_.GetAddressOf(), cpdepthview_.Get());

	//ラスタライザの設定
	rasterdesc.AntialiasedLineEnable = false;
	rasterdesc.CullMode = D3D11_CULL_BACK;
	rasterdesc.DepthBias = 0;
	rasterdesc.DepthBiasClamp = 0.0F;
	rasterdesc.DepthClipEnable = true;
	rasterdesc.FillMode = D3D11_FILL_SOLID;
	rasterdesc.FrontCounterClockwise = false;
	rasterdesc.MultisampleEnable = false;
	rasterdesc.ScissorEnable = false;
	rasterdesc.SlopeScaledDepthBias = 0.0F;

	//設定した内容からラスタライザを作成
	hr = cpdevice_.Get()->CreateRasterizerState(&rasterdesc, cprasterstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ラスタライザの状態を設定
	cpdevicecontext_.Get()->RSSetState(cprasterstate_.Get());

	//2番目のラスタライザの設定
	rasterdesc.AntialiasedLineEnable = false;
	rasterdesc.CullMode = D3D11_CULL_NONE;
	rasterdesc.DepthBias = 0;
	rasterdesc.DepthBiasClamp = 0.0F;
	rasterdesc.DepthClipEnable = true;
	rasterdesc.FillMode = D3D11_FILL_SOLID;
	rasterdesc.FrontCounterClockwise = false;
	rasterdesc.MultisampleEnable = false;
	rasterdesc.ScissorEnable = false;
	rasterdesc.SlopeScaledDepthBias = 0.0F;

	//ラスタライザを作成
	hr = cpdevice_.Get()->CreateRasterizerState(&rasterdesc, cprasterstatenoculling_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//ビューポートをレンダリング用にセットアップ
	viewport_.Height = static_cast<float>(ScreenHeight);
	viewport_.Width = static_cast<float>(ScreenWidth);
	viewport_.MinDepth = 0.0F;
	viewport_.MaxDepth = 1.0F;
	viewport_.TopLeftX = 0.0F;
	viewport_.TopLeftY = 0.0F;

	//ビューポートの作成
	setViewPort();

	//射影行列の作成
	fieldofview = XM_PI / 14.0F;
	screenaspect = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);

	//3Dレンダリングの投影行列を作成
	projection_ = XMMatrixPerspectiveFovLH(fieldofview, screenaspect, ScreenNear, ScreenDepth);

	//ワールド行列を初期化
	world_ = XMMatrixIdentity();

	//2Dレンダリング用の正投影行列を作成
	ortho_ = XMMatrixOrthographicLH(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), ScreenNear, ScreenDepth);
	
	return true;
}

void Direct3D::begin(XMVECTORF32 Color)
{

	//バックバッファクリア
	cpdevicecontext_.Get()->ClearRenderTargetView(cprendertarget_.Get(), Color);

	//深度バッファクリア
	cpdevicecontext_.Get()->ClearDepthStencilView(cpdepthview_.Get(), D3D11_CLEAR_DEPTH, 1.0F, 0);

}

void Direct3D::end()
{
	//レンダリングを完了してバックバッファに表示
	if (vsync_)
	{
		//垂直同期をしてリフレッシュレートをロック
		cpswapchain_.Get()->Present(1, 0);
	}
	else
	{
		//垂直同期をせずに最高速で表示
		cpswapchain_.Get()->Present(0, 1);
	}
}

void Direct3D::destroy()
{
	//インターフェースの解放忘れを出力
	typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
	HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");

	//正常にDLLをロードできていれば実行
	if (hDll != NULL)
	{
		fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface");
		DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&cpdxgidebug_);

		cpdevice_.Get()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(cpdebug_.GetAddressOf()));
		cpdebug_.Get()->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
}

void Direct3D::setVideoCardInfo(char* CardName, int& Memory)
{
	//ビデオカードの名前とメモリ容量を取得
	strcpy_s(CardName, 128, videocarddescription_);
	Memory = videocardmemory_;
}

void Direct3D::setBackBufferRenderTarget()
{
	//スロットをリセットするためのダミー情報を作成
	ID3D11RenderTargetView* dummyrtv[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
	ID3D11ShaderResourceView* dummysrv[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

	//ダミーの情報をセット
	cpdevicecontext_.Get()->OMSetRenderTargets(_countof(dummyrtv), dummyrtv, nullptr);
	cpdevicecontext_.Get()->PSSetShaderResources(0, _countof(dummysrv), dummysrv);

	//レンダーターゲットビューと深度ステンシルバッファをパイプラインにバインド
	setRenderTarget();
}

void Direct3D::setRenderTarget()
{
	cpdevicecontext_.Get()->OMSetRenderTargets(1, cprendertarget_.GetAddressOf(), cpdepthview_.Get());
}

void Direct3D::setViewPort()
{
	cpdevicecontext_.Get()->RSSetViewports(1, &viewport_);
}
