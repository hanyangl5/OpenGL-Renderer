#pragma once
#include "Camera.h"
#include "Framebuffer.h"
#include "Model.h"
#include "Shader.h"
#include "Skybox.h"
#include "utils.h"
#include <array>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include "Light.h"
class Deferrdpass {
public:
  Deferrdpass(uint32_t w, uint32_t h);
  ~Deferrdpass();
  void Draw(std::shared_ptr<Framebuffer> dst,
            std::unordered_map<std::string, std::shared_ptr<Model>> &scene,
            std::shared_ptr<Camera> cam,
      std::shared_ptr<Quad> quad, std::shared_ptr<Skybox> skybox, std::vector<std::shared_ptr<Light>>&lights);
  uint32_t PosTex() const;
  uint32_t NormalTex() const;
  uint32_t AlbedoTex() const;

private:
  uint32_t width, height;
  uint32_t gBuffer, rboDepth;
  uint32_t gPosition, gNormal, gAlbedo, gMetallicRoughness;
  std::shared_ptr<Shader> geopass_shader;
  std::shared_ptr<Shader> lightingpass_shader;
  std::array<uint32_t, 4> attachments;
};
