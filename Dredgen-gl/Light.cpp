#include "Light.h"

DirectLight::DirectLight(glm::vec3 c, glm::vec3 p, glm::vec3 d)
{
	color = c;
	pos = p;
	dir = d;
}

glm::vec3 DirectLight::GetColor() const
{
	return color;
}

glm::vec3 DirectLight::GetDir() const
{
	return dir;
}

glm::vec3 DirectLight::GetPos() const
{
	return pos;
}

LightType DirectLight::GetType() const
{
	return LightType::Direct;
}

PointLight::PointLight(glm::vec3 c, glm::vec3 p)
{
	color = c;
	pos = p;
}

glm::vec3 PointLight::GetColor() const
{
	return color;
}

glm::vec3 PointLight::GetDir() const
{
	return glm::vec3(0.0, 0.0, 0.0);
}

glm::vec3 PointLight::GetPos() const
{
	return pos;
}

LightType PointLight::GetType() const
{
	return LightType::Point;
}
