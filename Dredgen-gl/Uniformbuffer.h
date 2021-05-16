#pragma  once
#include <cstdint>
#include <memory>
#include "Shader.h"
#include "Light.h"
class UboLight
{
public:
	UboLight(std::shared_ptr<Shader> shader);
	~UboLight();
	void Update(std::shared_ptr<Light> light);
private:
	uint32_t ubo_light{};
	uint32_t uniform_block{};
};

