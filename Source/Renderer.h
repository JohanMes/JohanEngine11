#ifndef RENDERER_H
#define RENDERER_H

#include <D3D11.h>
#include "Clock.h"
#include "Resource.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Renderer {
	ID3D11Device* D3DDevice;
	ID3D11DeviceContext* DeviceContext;
	IDXGISwapChain* SwapChain;
	ID3D11Texture2D* DepthStencilBuffer;
	ID3D11RenderTargetView* RenderTargetView;
	ID3D11DepthStencilView* DepthStencilView;
	HWND hwnd;
	Clock* clock;
	bool paused;
	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateRenderTargetView();
	bool CreateDepthStencilSurface();
	public:
		Renderer(HWND hwnd);
		~Renderer();
		
		HWND GetHWND();
		void SetHWND(HWND hwnd);
		bool SetRenderTarget(int NumViews,ID3D11RenderTargetView* RenderTarget,ID3D11DepthStencilView* RenderTargetDepth);
		void RenderFrame();
		bool OnResize();
};

extern Renderer* renderer;

#endif
