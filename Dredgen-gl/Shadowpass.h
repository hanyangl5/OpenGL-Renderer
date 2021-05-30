#pragma once
#include "Light.h"
#include "Model.h"
#include "Shader.h"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
class Shadowpass {
public:
  Shadowpass() = default;
  Shadowpass(uint32_t w,uint32_t h);
  ~Shadowpass();
  uint32_t GetShadowTex() const;
  void Draw(std::unordered_map<std::string, std::shared_ptr<Model>> &scene, std::vector<std::shared_ptr<Light>>&lights);

private:
  uint32_t shadow_fbo, shadow_tex;
  const uint32_t width = 512, height = 512;
  std::shared_ptr<Shader> shadowshader{};
};
