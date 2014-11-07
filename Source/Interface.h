#ifndef INTERFACE_H
#define INTERFACE_H

#include "Resource.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

class DLLIMPORT Interface {
	HWND hwnd;
	public:
		Interface(HWND hwnd);
		~Interface();
};

#undef interface
extern Interface* interface;

#endif
