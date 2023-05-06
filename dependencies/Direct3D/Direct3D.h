#pragma once
#include <cstdint>
#include <dxgi.h>
#include <d3d11.h>

class CDirect3D
{
public:
	void* Present;
	void* ResizeBuffers;
	bool Initialization();
	void Destroy();
};

inline CDirect3D Direct3D;

