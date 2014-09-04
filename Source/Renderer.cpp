#include "Renderer.h"

Renderer::Renderer() {
	D3D_FEATURE_LEVEL RequiredLevel = D3D_FEATURE_LEVEL_11_0; 
	D3D_FEATURE_LEVEL SupportedLevel;
	D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE, // we want hardware acceleration
		NULL, // pointer to software renderer. don't use that
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		&RequiredLevel,
		1,
		D3D11_SDK_VERSION,
		&Device,
		&SupportedLevel,
		&DeviceContext
		);
}

Renderer::~Renderer() {
	
}
