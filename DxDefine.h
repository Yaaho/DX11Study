#pragma once

// linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


// include
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "DDSTextureLoader.h"
using namespace DirectX;

// warning C4316 ó����
#include "AlignedAllocationPolicy.h"