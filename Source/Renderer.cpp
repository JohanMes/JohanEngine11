#include "Renderer.h"

Renderer::Renderer(HWND hwnd) {
	this->hwnd = hwnd; // parent window
	D3DDevice = NULL;
	DeviceContext = NULL;
	RenderTargetView = NULL;
	SwapChain = NULL;
	DepthStencilBuffer = NULL;
	DepthStencilView = NULL;
	
	// Initialize and check for DX11 support
	if(!CreateDevice()) {
		return;
	}
	
	// Create swap chain
	if(!CreateSwapChain()) {
		return;
	}

	// Create view for back buffer we just created
	if(!CreateDepthStencilSurface()) {
		return;
	}
		
	// Create view for back buffer we just created
	if(!CreateRenderTargetView()) { // also creates views for DepthStencil
		return;
	}
	
	// Use them as targets
	if(!SetRenderTarget(1,RenderTargetView,DepthStencilView)) {
		return;
	}
	
	// Start timer
	clock = new Clock(true);
	paused = false;
}

Renderer::~Renderer() {
	delete clock;
	SetRenderTarget(0,NULL,NULL);
	DepthStencilView->Release();
	DepthStencilBuffer->Release();
	SwapChain->Release();
	RenderTargetView->Release();
	DeviceContext->Release();
	D3DDevice->Release();
}

bool Renderer::CreateDevice() {
	D3D_FEATURE_LEVEL SupportedLevel;
	if(D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE, // we want hardware acceleration
		NULL, // pointer to software renderer. don't use that
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&D3DDevice,
		&SupportedLevel,
		&DeviceContext
	) != S_OK) {
		MessageBox(hwnd,"Error creating device","Error",MB_OK);
		return false;
	}
	
	// Exit if it's not supported
	if(SupportedLevel != D3D_FEATURE_LEVEL_11_0) {
		MessageBox(hwnd,"Your computer does not support DirectX 11","Error",MB_OK);
		return false;
	}
	
	return true;
}

bool Renderer::CreateSwapChain() {
	// Determine buffer size
	RECT rect;
	GetClientRect(hwnd,&rect);
	
	// Create swap chain
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Width = rect.right - rect.left;
	SwapChainDesc.BufferDesc.Height = rect.bottom - rect.top;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1; // 60/1 = 60 Hz
	SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.OutputWindow = hwnd;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0; // no  AA
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	SwapChainDesc.Windowed = true;
	
	// Obtain DXGI factory that was used to create the device
	// ???
	IDXGIDevice* DXGIDevice;
	D3DDevice->QueryInterface(__uuidof(IDXGIDevice),(void**)&DXGIDevice);
	IDXGIAdapter* DXGIAdapter;
	DXGIDevice->GetParent(__uuidof(IDXGIAdapter),(void**)&DXGIAdapter);
	IDXGIFactory* DXGIFactory;
	DXGIAdapter->GetParent(__uuidof(IDXGIFactory),(void**)&DXGIFactory);

	// Use it
	if(DXGIFactory->CreateSwapChain(D3DDevice,&SwapChainDesc,&SwapChain) != S_OK) {
		MessageBox(hwnd,"Error creating swap chain","Error",MB_OK);
		return false;
	}
	
	// Release unused stuff
	DXGIDevice->Release();
	DXGIAdapter->Release();
	DXGIFactory->Release();
	return true;
}

bool Renderer::CreateRenderTargetView() {
	// Get back buffer
	ID3D11Texture2D* BackBuffer;
	SwapChain->GetBuffer(0,__uuidof(ID3D11Texture2D),(void**)&BackBuffer);
	
	// Create a view for it
	if(D3DDevice->CreateRenderTargetView(BackBuffer,NULL,&RenderTargetView) != S_OK) {
		MessageBox(hwnd,"Error creating render target view","Error",MB_OK);
		return false;
	}
	if(D3DDevice->CreateDepthStencilView(DepthStencilBuffer,NULL,&DepthStencilView)) {
		MessageBox(hwnd,"Error creating depth stencil view","Error",MB_OK);
		return false;
	}
		
	BackBuffer->Release();
	
	return true;
}

bool Renderer::CreateDepthStencilSurface() {
	// Determine buffer size
	RECT rect;
	GetClientRect(hwnd,&rect);
	
	// Create regular texture that functions as a depth buffer
	D3D11_TEXTURE2D_DESC TextureDesc;
	TextureDesc.ArraySize = 1;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	TextureDesc.Width = rect.right - rect.left;
	TextureDesc.Height = rect.bottom - rect.top;
	TextureDesc.MipLevels = 1;
	TextureDesc.MiscFlags = 0;
	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	if(D3DDevice->CreateTexture2D(&TextureDesc,NULL,&DepthStencilBuffer)) {
		MessageBox(hwnd,"Error creating deoth stencil surface","Error",MB_OK);
		return false;
	}

	return true;
}

bool Renderer::SetRenderTarget(int NumViews,ID3D11RenderTargetView* RenderTargetView,ID3D11DepthStencilView* RenderTargetDepthView) {
	DeviceContext->OMSetRenderTargets(NumViews,&RenderTargetView,RenderTargetDepthView); // no return value
	return true;
}

HWND Renderer::GetHWND() {
	return hwnd;
}

void Renderer::SetHWND(HWND hwnd) {
	if(this->hwnd != hwnd) {
		this->hwnd = hwnd;
		//CreateSwapChain();
	}
}

bool Renderer::OnResize() {
	// Render target setting creates references
	if(!SetRenderTarget(0,NULL,NULL)) {
		return false;
	}
	
	// Release views to back buffer
	RenderTargetView->Release();
	DepthStencilView->Release();
	DepthStencilBuffer->Release();
	
	// Then resize
	SwapChain->ResizeBuffers(1,0,0,DXGI_FORMAT_UNKNOWN,DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// Create view for back buffer we just created
	if(!CreateDepthStencilSurface()) {
		return false;
	}
	
	// Recreate views
	if(!CreateRenderTargetView()) {
		return false;
	}
	
	// Reset render target
	if(!SetRenderTarget(1,RenderTargetView,DepthStencilView)) {
		return false;
	}

	return true;
}

void Renderer::RenderFrame() {
	if(paused) {
		return;
	}
	
	double FrameTimeSec = clock->Reset();
	
	// Clear back buffer
	float ClearColor[4] = {0.0f,0.0f,0.0f,0.0f};
	DeviceContext->ClearRenderTargetView(RenderTargetView,ClearColor);
	
	// Push to monitor
	SwapChain->Present(0,0);
}
