// #include "Editor/Editor.h"
#include <iostream>
#include <memory>
#include "GFX/gfx.h"
#include "GFX/OpenGL/gfx_ogl.h"
#include "GFX/DX11/gfx_dx11.h"
#include "Window/window.h"

const uint32_t w = 1800, h = 1000;

int main() {
    std::unique_ptr<Window> window = std::make_unique<Window>(w, h, "name");
    
    std::unique_ptr<GFX> gfx_dx11 = std::make_unique<DX11>();
    std::unique_ptr<GFX> gfx_ogl = std::make_unique<OGL>();
    // Editor editor{"dredgen renderer", w, h};

    // try {
    //     editor.Run();
    // } catch (const std::exception &e) {
    //     std::cerr << e.what() << std::endl;
    //     return 1;
    // }

    return 0;
}