#ifndef RENDERER_H
#define RENDERER_H

#include <D3D11.h>

class Renderer {
	ID3D11Device* Device;
	ID3D11DeviceContext* DeviceContext;
	public:
		Renderer();
		~Renderer();
};

#endif
