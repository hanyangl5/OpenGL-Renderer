#include "Editor/Editor.h"
#include <iostream>

const uint32_t w = 1800, h = 1000;

int main() {
    Editor editor{"dredgen renderer", w, h};

    try {
        editor.Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}