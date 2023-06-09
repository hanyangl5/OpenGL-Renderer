#pragma once
#include "../gfx.h"
#include "Utils/Log.h"
#include <glad.h>

class OGL : public GFX {
    OGL() {

        int version = gladLoadGL();
        if (version == 0) {
            Log::Err("failed to initialize opengl");
        }

        //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        //    Log::Err("Failed to initialize GLAD");
        //} else {
        //    Log::Log("glad inited\n");
        //}
    }
};
