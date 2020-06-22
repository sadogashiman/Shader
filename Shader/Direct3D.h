#pragma once
class Direct3D
{
private:
	Direct3D();
	~Direct3D();
	bool vsync_;
	int videocardmemory_;
	char videocarddescription_[128];
	Matrix projection_;
	Matrix world_;
	Matrix ortho_;

	//インターフェース
	ComPtr<ID3D11Device> cpdevice_;
	ComPtr<ID3D11DeviceContext> cpdevicecontext_;
	ComPtr<ID3D11RenderTargetView> cprendertarget_;
	ComPtr<ID3D11DepthStencilState> cpdepthstate_;
	ComPtr<ID3D11DepthStencilView> cpdepthview_;
	ComPtr<ID3D11RasterizerState> cprasterstate_;
	ComPtr<ID3D11RasterizerState> cprasterstatenoculling_;
	ComPtr<ID3D11RasterizerState> cprasterstatewireframe_;
	ComPtr<ID3D11Texture2D> cpdepthstencilbuffer_;
	ComPtr<ID3D11BlendState> cpenabledalphablendstate_;
	ComPtr<ID3D11BlendState> cpdisabledalphablendstate_;
	ComPtr<ID3D11BlendState> cpaddalphablendstate_;
	ComPtr<ID3D11DepthStencilState> cpdepthdisabledstate_;
	ComPtr<ID3D11Debug> cpdebug_;
	ComPtr<IDXGISwapChain> cpswapchain_;
	ComPtr<IDXGIDebug> cpdxgidebug_;
	D3D11_VIEWPORT viewport_;

public:
	Direct3D(const Direct3D&) = delete;
	Direct3D& operator = (const Direct3D&) = delete;
	Direct3D(Direct3D&&) = delete;
	Direct3D& operator = (Direct3D&&) = delete;

	bool init(const int ScreenWidth, const int ScreenHeight, const bool Vsync,const bool FullScreen, const float ScreenDepth, const float ScreenNear);
	void begin(XMVECTORF32 Color);
	void end();
	void destroy();

	//get
	inline ID3D11Device* getDevice()const { return cpdevice_.Get(); }
	inline ID3D11DeviceContext* getContext()const { return cpdevicecontext_.Get(); }
	inline ID3D11DepthStencilView* getStencilView()const { return cpdepthview_.Get(); }
	inline ID3D11RenderTargetView* getRenderTargetView()const { return cprendertarget_.Get(); }
	inline IDXGISwapChain* getSwapChain()const { return cpswapchain_.Get(); }

	inline XMMATRIX getProjection()const { return projection_; }
	inline XMMATRIX getWorld()const { return world_; }
	inline XMMATRIX getOrtho()const { return ortho_; }

	inline const char* getVideoCardName()const { return videocarddescription_; }	//GPUの名前を取得する
	inline const int getVideoCardMemroy()const { return videocardmemory_; }			//GPUのメモリーを取得する
	
	//set
	void setVideoCardInfo(char* CardName, int& Memory);
	void setBackBufferRenderTarget();
	inline void setRenderTarget(){cpdevicecontext_.Get()->OMSetRenderTargets(1, cprendertarget_.GetAddressOf(), cpdepthview_.Get());}

	//reset
	inline void resetViewPort() { cpdevicecontext_.Get()->RSSetViewports(1, &viewport_); }

	//Zbuffer切り替え関数
	inline void turnZbufferEnable() { cpdevicecontext_.Get()->OMSetDepthStencilState(cpdepthstate_.Get(), 1); }
	inline void turnZbufferDisable() { cpdevicecontext_.Get()->OMSetDepthStencilState(cpdepthdisabledstate_.Get(), 1); }

	//Culling切り替え関数
	inline void turnCullingEnable() { cpdevicecontext_.Get()->RSSetState(cprasterstate_.Get()); }
	inline void turnCullingDisable() { cpdevicecontext_.Get()->RSSetState(cprasterstatenoculling_.Get()); }

	//WireFrameレンダリング切り替え関数
	inline void wireFrameEnable() { cpdevicecontext_.Get()->RSSetState(cprasterstatewireframe_.Get()); }
	inline void wireFrameDisable() { cpdevicecontext_.Get()->RSSetState(cprasterstate_.Get()); }

	//ブレンド切り替え関数
	void turnAddBlendEnable();
	void turnAlphaBlendEnable();
	void turnAlphablendDisable();

	//static
	static inline Direct3D* getInstance()
	{
		static Direct3D instance;
		return &instance;
	}

};

