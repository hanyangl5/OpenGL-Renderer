#include <glad/glad.h>
#include "Deferrdpass.h"


const unsigned int NR_LIGHTS = 32;
std::vector<glm::vec3> lightPositions;
std::vector<glm::vec3> lightColors;
// srand(13);

Deferrdpass::Deferrdpass(uint32_t w, uint32_t h) : width(w), height(h) {
  for (unsigned int i = 0; i < NR_LIGHTS; i++) {
    // calculate slightly random offsets
    float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
    float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
    float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
    lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
    // also calculate random color
    float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
    float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
    float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
    lightColors.push_back(glm::vec3(rColor, gColor, bColor));
  }
  geopass_shader =
      std::make_shared<Shader>("../resources/shaders/geopassvs.glsl",
                               "../resources/shaders/geopassps.glsl");
  lightingpass_shader =
      std::make_shared<Shader>("../resources/shaders/lightingpassvs.glsl",
                               "../resources/shaders/lightingpassps.glsl");

  glGenFramebuffers(1, &gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

  // pos
  glGenTextures(1, &gPosition);
  glBindTexture(GL_TEXTURE_2D, gPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);

  // normal+depth
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // albedo
  glGenTextures(1, &gAlbedo);
  glBindTexture(GL_TEXTURE_2D, gAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedo, 0);

  glGenTextures(1, &gMetallicRoughness);
  glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
	  GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D,
      gMetallicRoughness, 0);

  // - tell OpenGL which color attachments we'll use (of this framebuffer) for
  // rendering

  attachments = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                 GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };

  glDrawBuffers(4, attachments.data());

  // generate depth buffer
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, rboDepth);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);


  lightingpass_shader->use();
  lightingpass_shader->setInt("gPosition", 0);
  lightingpass_shader->setInt("gNormal", 1);
  lightingpass_shader->setInt("gAlbedo", 2);
  lightingpass_shader->setInt("gMetallicRoughness", 3);

}

Deferrdpass::~Deferrdpass() {

  // glDeleteBuffers(GL_FRAMEBUFFER, &gBuffer);
  // glDeleteRenderbuffers(GL_RENDERBUFFER, &rboDepth);
  // glDeleteTextures(GL_TEXTURE_2D, &gAlbedoSpec);
  // glDeleteTextures(GL_TEXTURE_2D, &gNormal);
  // glDeleteTextures(GL_TEXTURE_2D, &gPosition);
}

// dst frame buffer, scene, cam, skybox
void Deferrdpass::Draw(
    std::shared_ptr<Framebuffer> dst,
    std::unordered_map<std::string, std::shared_ptr<Model>> &scene,
    std::shared_ptr<Camera> cam, std::shared_ptr<Quad> quad) {
  glViewport(0, 0, width, height);

  // geometry pass
  {
    // configure
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    geopass_shader->use();
    geopass_shader->setMat4("projection", cam->projection);
    geopass_shader->setMat4("view", cam->GetViewMatrix());
    geopass_shader->setVec3("viewPos", cam->Position);
    for (auto &i : scene) {
      i.second->Draw(*geopass_shader.get(), i.second->rendermode);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  // lighting pass
  {

    glBindFramebuffer(GL_FRAMEBUFFER, dst->fbo);
    glViewport(0, 0, width, height);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    lightingpass_shader->use();
    lightingpass_shader->setVec3("viewPos", cam->Position);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);

    // send light relevant uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
      lightingpass_shader->setVec3("lights[" + std::to_string(i) + "].Position",
                                   lightPositions[i]);
      lightingpass_shader->setVec3("lights[" + std::to_string(i) + "].Color",
                                   lightColors[i]);
    }
    glm::vec3 light_dir(-1.0, -1.0, -1.0);
    glm::vec3 light_color(1.0,0.9568,0.4392);

    lightingpass_shader->setVec3("directlight.Direction", light_dir);
    lightingpass_shader->setVec3("directlight.Color", 2.0f*light_color);
    quad->Draw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

uint32_t Deferrdpass::PosTex() const { return gPosition; }

uint32_t Deferrdpass::NormalTex() const { return gNormal; }

uint32_t Deferrdpass::AlbedoTex() const { return gAlbedo; }
