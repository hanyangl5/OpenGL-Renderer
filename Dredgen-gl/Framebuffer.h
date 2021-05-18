#pragma once
#include <array>
#include <cstdint>
class Framebuffer {
public:
  Framebuffer(uint32_t w, uint32_t h);
  ~Framebuffer();

  // private:
  std::array<uint32_t, 1> data;
  uint32_t fbo, tex, rbo;
  uint32_t width, height;
};
