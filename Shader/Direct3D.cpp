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

	//���������ݒ�
	vsync_ = Vsync;

	//DirectX�O���t�B�b�N�X�C���^�[�t�F�C�X���쐬
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factry);
	if (FAILED(hr))
	{
		return false;
	}

	//�t�@�N�g�����g�p���ăv���C�}���O���t�B�b�N�X�C���^�[�t�F�C�X(�r�f�I�J�[�h)�p�̃A�_�v�^�[���쐬
	hr = factry->EnumAdapters(0, &adapter);
	if (FAILED(hr))
	{
		return false;
	}

	//�v���C�}���A�_�v�^�[�̏o��(���j�^�[)���
	hr = adapter->EnumOutputs(0, &adapteroutput);
	if (FAILED(hr))
	{
		return false;
	}

	//�A�_�v�^�[�o�͂̓K�����[�h�̐����擾
	hr = adapteroutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nummodes, NULL);
	if (FAILED(hr))
	{
		return false;
	}

	//���j�^�[�ƃr�f�I�J�[�h�̑g�ݍ��킹�ŉ\�Ȃ��ׂĂ̕\�����[�h��ێ����郊�X�g���쐬
	//�r�f�I�J�[�h����������ꍇ�ō��ݒ�ł�����̂��g�p
	displaymodelist = new DXGI_MODE_DESC[nummodes];
	if (!displaymodelist)
	{
		return false;
	}

	//�\�����[�h�̃��X�g���쐬
	hr = adapteroutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &nummodes, displaymodelist);
	if (FAILED(hr))
	{
		return false;
	}

	//���������\�����[�h�̒������ʕ��ƍ����Ɉ�v������̂�������
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

	//�A�_�v�^���擾
	hr = adapter->GetDesc(&adapterdesc);
	if (FAILED(hr))
	{
		return false;
	}

	//�r�f�I�J�[�h�̃����������K�o�C�g�P�ʂŊm��
	videocardmemory_ = static_cast<int>(adapterdesc.DedicatedVideoMemory / 1024 / 1024);

	//�r�f�I�J�[�h�̖��O�𕶎���ɕϊ����ĕۑ�
	error = wcstombs_s(&stringlength, videocarddescription_, 128, adapterdesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	//�\�����[�h�̃��X�g�����
	delete[] displaymodelist;
	displaymodelist = nullptr;

	//�A�_�v�^�[�o�͂����
	adapter->Release();
	adapter = nullptr;

	//�t�@�N�g���[�����
	factry->Release();
	factry = nullptr;


	//****************************************
	//        DirectX�̏�����
	//****************************************

	//�X���b�v�`�F�[���̏�����
	ZeroMemory(&swapchaindesc, sizeof(swapchaindesc));

	//�o�b�N�o�b�t�@�̐ݒ�
	swapchaindesc.BufferCount = 1;

	//�o�b�N�o�b�t�@�̕��ƍ�����ݒ�
	swapchaindesc.BufferDesc.Width = ScreenWidth;
	swapchaindesc.BufferDesc.Height = ScreenHeight;

	//�o�b�N�o�b�t�@��32�r�b�g�T�[�t�F�C�X��ݒ�
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//�o�b�N�o�b�t�@�̃��t���b�V�����[�g��ݒ�
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

	//�o�b�N�o�b�t�@�̎g�p���@��ݒ�
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//�����_�����O����E�B���h�E�̎g�p���@��ݒ�
	swapchaindesc.OutputWindow = Hwnd;

	//�}���`�T���v�����O�̐ݒ�
	swapchaindesc.SampleDesc.Count = 1;
	swapchaindesc.SampleDesc.Quality = 0;

	//�E�B���h�E���[�h�̐ݒ�
	if (FullScreen)
	{
		swapchaindesc.Windowed = false;
	}
	else
	{
		swapchaindesc.Windowed = true;
	}

	//�X�L�������C���̏����ƃX�P�[�����O�̐ݒ�
	swapchaindesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapchaindesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//�\��������o�b�N�o�b�t�@��j������悤�ݒ�
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//�A�h�o���X�t���O�̐ݒ�
	swapchaindesc.Flags = 0;

	//�X���b�v�`�F�[���EDirect3D�f�o�C�X�EDirect3D�f�o�C�X�R���e�L�X�g�̍쐬

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
		//�ݒ肵���ǂ̋@�\���x���ł��ݒ肪�ł��Ȃ��ꍇ�����CPU���g�p����(�������x���ቺ����)
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
			//����ł��ݒ�ł��Ȃ��ꍇ�G���[�Ƃ��ď�������
			return false;
		}
	}

	//�o�b�N�o�b�t�@�ւ̃|�C���^���擾
	hr = cpswapchain_.Get()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbufferptr);
	if (FAILED(hr))
	{
		return false;
	}

	//�o�b�N�o�b�t�@�̃|�C���^�Ń����_�[�^�[�Q�b�g�r���[���쐬
	hr = cpdevice_.Get()->CreateRenderTargetView(backbufferptr, NULL, cprendertarget_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�s�v�ɂȂ����o�b�N�o�b�t�@�ւ̃|�C���^�����
	backbufferptr->Release();
	backbufferptr = nullptr;


	//�[�x�o�b�t�@�̏�����
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

	//�ݒ肵�����e���g���Đ[�x�o�b�t�@�̃e�N�X�`�����쐬
	hr = cpdevice_.Get()->CreateTexture2D(&depthbufferdesc, NULL, cpdepthstencilbuffer_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�X�e���V���̏�����
	ZeroMemory(&depthstencildesc, sizeof(depthstencildesc));

	//�X�e���V���̐ݒ�
	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthstencildesc.StencilEnable = true;
	depthstencildesc.StencilReadMask = 0xFF;
	depthstencildesc.StencilWriteMask = 0xFF;

	//�s�N�Z�����O�������Ă��鎞�̃X�e���V������
	depthstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�s�N�Z�������������Ă��鎞�̃X�e���V������
	depthstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�[�x�X�e���V���̑���
	hr = cpdevice_.Get()->CreateDepthStencilState(&depthstencildesc, cpdepthstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�[�x�X�e���V���̏�Ԃ�ݒ�
	cpdevicecontext_.Get()->OMSetDepthStencilState(cpdepthstate_.Get(), 1);

	//��Ԗڂ̃X�e���V����������
	ZeroMemory(&depthdisabledstencildesc, sizeof(depthdisabledstencildesc));

	//�X�e���V���̐ݒ�
	depthdisabledstencildesc.DepthEnable = true;
	depthdisabledstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthdisabledstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthdisabledstencildesc.StencilEnable = true;
	depthdisabledstencildesc.StencilReadMask = 0xFF;
	depthdisabledstencildesc.StencilWriteMask = 0xFF;

	//�s�N�Z�����O�������Ă��鎞�̃X�e���V������
	depthdisabledstencildesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthdisabledstencildesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//�s�N�Z�������������Ă��鎞�̃X�e���V������
	depthdisabledstencildesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthdisabledstencildesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthdisabledstencildesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = cpdevice_.Get()->CreateDepthStencilState(&depthdisabledstencildesc, cpdepthdisabledstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�u�����h�̏�Ԃ��N���A
	ZeroMemory(&blendstatedesc, sizeof(D3D11_BLEND_DESC));

	//�ݒ��ύX���ăA���t�@�𖳌���
	blendstatedesc.RenderTarget[0].BlendEnable = FALSE;

	//�ݒ���g�p���ău�����h�X�e�[�g���쐬
	hr = cpdevice_.Get()->CreateBlendState(&blendstatedesc, cpenabledalphablendstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�[�x�X�e���V���r���[�̏�����
	ZeroMemory(&depthstencilviewdesc, sizeof(depthstencilviewdesc));

	depthstencilviewdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthstencilviewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthstencilviewdesc.Texture2D.MipSlice = 0;

	//�[�x�X�e���V���r���[�̍쐬
	hr = cpdevice_.Get()->CreateDepthStencilView(cpdepthstencilbuffer_.Get(), &depthstencilviewdesc, cpdepthview_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}


	//�����_�[�^�[�Q�b�g�Ɛ[�x�X�e���V���o�b�t�@���o�̓����_�[�p�C�v���C���Ƀo�C���h
	cpdevicecontext_.Get()->OMSetRenderTargets(1, cprendertarget_.GetAddressOf(), cpdepthview_.Get());

	//���X�^���C�U�̐ݒ�
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

	//�ݒ肵�����e���烉�X�^���C�U���쐬
	hr = cpdevice_.Get()->CreateRasterizerState(&rasterdesc, cprasterstate_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//���X�^���C�U�̏�Ԃ�ݒ�
	cpdevicecontext_.Get()->RSSetState(cprasterstate_.Get());

	//2�Ԗڂ̃��X�^���C�U�̐ݒ�
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

	//���X�^���C�U���쐬
	hr = cpdevice_.Get()->CreateRasterizerState(&rasterdesc, cprasterstatenoculling_.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	//�r���[�|�[�g�������_�����O�p�ɃZ�b�g�A�b�v
	viewport_.Height = static_cast<float>(ScreenHeight);
	viewport_.Width = static_cast<float>(ScreenWidth);
	viewport_.MinDepth = 0.0F;
	viewport_.MaxDepth = 1.0F;
	viewport_.TopLeftX = 0.0F;
	viewport_.TopLeftY = 0.0F;

	//�r���[�|�[�g�̍쐬
	setViewPort();

	//�ˉe�s��̍쐬
	fieldofview = XM_PI / 14.0F;
	screenaspect = static_cast<float>(ScreenWidth) / static_cast<float>(ScreenHeight);

	//3D�����_�����O�̓��e�s����쐬
	projection_ = XMMatrixPerspectiveFovLH(fieldofview, screenaspect, ScreenNear, ScreenDepth);

	//���[���h�s���������
	world_ = XMMatrixIdentity();

	//2D�����_�����O�p�̐����e�s����쐬
	ortho_ = XMMatrixOrthographicLH(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight), ScreenNear, ScreenDepth);
	
	return true;
}

void Direct3D::begin(XMVECTORF32 Color)
{

	//�o�b�N�o�b�t�@�N���A
	cpdevicecontext_.Get()->ClearRenderTargetView(cprendertarget_.Get(), Color);

	//�[�x�o�b�t�@�N���A
	cpdevicecontext_.Get()->ClearDepthStencilView(cpdepthview_.Get(), D3D11_CLEAR_DEPTH, 1.0F, 0);

}

void Direct3D::end()
{
	//�����_�����O���������ăo�b�N�o�b�t�@�ɕ\��
	if (vsync_)
	{
		//�������������ă��t���b�V�����[�g�����b�N
		cpswapchain_.Get()->Present(1, 0);
	}
	else
	{
		//���������������ɍō����ŕ\��
		cpswapchain_.Get()->Present(0, 1);
	}
}

void Direct3D::destroy()
{
	//�C���^�[�t�F�[�X�̉���Y����o��
	typedef HRESULT(__stdcall* fPtr)(const IID&, void**);
	HMODULE hDll = GetModuleHandleW(L"dxgidebug.dll");

	//�����DLL�����[�h�ł��Ă���Ύ��s
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
	//�r�f�I�J�[�h�̖��O�ƃ������e�ʂ��擾
	strcpy_s(CardName, 128, videocarddescription_);
	Memory = videocardmemory_;
}

void Direct3D::setBackBufferRenderTarget()
{
	//�X���b�g�����Z�b�g���邽�߂̃_�~�[�����쐬
	ID3D11RenderTargetView* dummyrtv[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
	ID3D11ShaderResourceView* dummysrv[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = { nullptr };

	//�_�~�[�̏����Z�b�g
	cpdevicecontext_.Get()->OMSetRenderTargets(_countof(dummyrtv), dummyrtv, nullptr);
	cpdevicecontext_.Get()->PSSetShaderResources(0, _countof(dummysrv), dummysrv);

	//�����_�[�^�[�Q�b�g�r���[�Ɛ[�x�X�e���V���o�b�t�@���p�C�v���C���Ƀo�C���h
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
