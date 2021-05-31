#include <glad/glad.h>
#include "Deferrdpass.h"
#include "Light.h"



Deferrdpass::Deferrdpass(uint32_t w, uint32_t h) : width(w), height(h) {

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPosition, 0);

  // normal+depth
  glGenTextures(1, &gNormal);
  glBindTexture(GL_TEXTURE_2D, gNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormal, 0);

  // albedo
  glGenTextures(1, &gAlbedo);
  glBindTexture(GL_TEXTURE_2D, gAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedo, 0);

  glGenTextures(1, &gMetallicRoughness);
  glBindTexture(GL_TEXTURE_2D, gMetallicRoughness);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
	  GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
  lightingpass_shader->setInt("shadow_map", 4);
  lightingpass_shader->unuse();

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
    std::shared_ptr<Camera> cam, std::shared_ptr<Quad> quad,
	std::shared_ptr<Skybox> skybox, std::vector<std::shared_ptr<Light>>& lights,
	uint32_t shadowmap) {
  glViewport(0, 0, width, height);

  // geometry pass
  {
    // configure
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    geopass_shader->use();
    geopass_shader->setMat4("projection", cam->projection);
    geopass_shader->setMat4("view", cam->GetViewMatrix());
    geopass_shader->setVec3("viewPos", cam->Position);
    for (auto &i : scene) {
      i.second->Draw(*geopass_shader.get(), i.second->rendermode);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    geopass_shader->unuse();
  }
  //skybox->Draw(dst, cam->projection, glm::mat3(cam->GetViewMatrix()), NormalTex());
  // send light to shader
  {
      lightingpass_shader->use();
      uint8_t point_light_index = 0;
      uint8_t direct_light_index = 0;

      for (auto l : lights) {
		  switch (l->GetType())
		  {
		  case LightType::Direct:
			  lightingpass_shader->setVec3("direct_lights[" + std::to_string(direct_light_index) + "].Direction", l->GetDir());
			  lightingpass_shader->setVec3("direct_lights[" + std::to_string(direct_light_index) + "].Color", l->GetColor());
              direct_light_index++;
			  break;
		  case LightType::Point:
			  lightingpass_shader->setVec3("point_lights[" + std::to_string(point_light_index) + "].Position", l->GetPos());
			  lightingpass_shader->setVec3("point_lights[" + std::to_string(point_light_index) + "].Color", l->GetColor());
              point_light_index++;
			  break;
		  case LightType::Spot:
			  break;
		  default:
			  break;
		  }
      }

	  float near_plane = 1.0f, far_plane = 70.5f;
	  glm::mat4 lightProjection =
		  glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
	  glm::mat4 lightView = glm::lookAt(glm::vec3(5.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
		  glm::vec3(0.0f, 1.0f, 0.0f));
	  glm::mat4 lightSpaceMatrix = lightProjection * lightView;
      lightingpass_shader->setMat4("lightMVP",lightSpaceMatrix);

      lightingpass_shader->unuse();
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
    glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowmap);

    quad->Draw();
    lightingpass_shader->unuse();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  // skybox
  {
	  //skybox->Draw(dst, cam->projection, glm::mat3(cam->GetViewMatrix()), NormalTex(), AlbedoTex());
  }
}

uint32_t Deferrdpass::PosTex() const { return gPosition; }

uint32_t Deferrdpass::NormalTex() const { return gNormal; }

uint32_t Deferrdpass::AlbedoTex() const { return gAlbedo; }
