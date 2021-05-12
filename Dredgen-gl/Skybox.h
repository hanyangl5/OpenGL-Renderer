#pragma  once
#include <cstdint>
#include "Shader.h"
#include <memory>
#include <string>
class Skybox
{
public:
	Skybox(std::string dirpath);
	~Skybox();
	void Draw(glm::mat4 projmat, glm::mat3 viewmat);
private:
	uint32_t skybox_texture, skybox_vao	, skybox_vbo;;
	std::shared_ptr <Shader> skyboxshader;
	//Shader skyboxshader;
};
