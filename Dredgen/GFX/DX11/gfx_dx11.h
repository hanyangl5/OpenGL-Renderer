#pragma once

#include <cassert>
#include <d3d11_1.h>
#include <windows.h>

#pragma comment(lib, "d3d11.lib")

#include <GLFW/glfw3.h>

#include "GFX/gfx.h"
#include "Utils/Log.h"

class DX11 : public GFX {
  public:
    DX11();
    virtual ~DX11();
};
