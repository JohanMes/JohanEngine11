#include "Renderer.h"
#include "Interface.h"

#if BUILDING_DLL
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

DLLIMPORT class Interface* interface = NULL;
DLLIMPORT class Renderer* renderer = NULL;
