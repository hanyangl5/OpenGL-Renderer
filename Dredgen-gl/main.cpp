#include <iostream>
#include "Editor.h"

const uint32_t w = 1800, h = 1000;

int main(int argc, char *argv[]) {
  Editor editor{"dredgen graphic engine", w, h};

  try {
    editor.Run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}