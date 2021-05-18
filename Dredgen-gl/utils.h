#pragma once
#include <cstdint>

class Quad {
public:
  Quad();
  ~Quad();
  void Draw();
  uint32_t quad_vao, quad_vbo;
};