#pragma once
#include <glm/glm.hpp>
enum class LightType
{
    Direct,
    Point,
    Spot
};

class Light
{
public:
    Light() = default;
    virtual glm::vec3 GetColor() const = 0;
    virtual glm::vec3 GetDir()const=0;
    virtual glm::vec3 GetPos() const = 0;
    virtual LightType GetType() const = 0;

    protected:
    glm::vec3 color;
    glm::vec3 pos, dir;
};

class DirectLight : public Light
{
    public:
    DirectLight(glm::vec3 c,glm::vec3 p,glm::vec3 d);
    glm::vec3 GetColor() const override;
    glm::vec3 GetDir()const override;
    glm::vec3 GetPos() const override;
    LightType GetType() const override;
};

// the point light dir is (0,0,0)
class PointLight : public Light
{
public:
    PointLight(glm::vec3 c, glm::vec3 p);
    glm::vec3 GetColor() const override;
    glm::vec3 GetDir()const override;
    glm::vec3 GetPos() const override;
    LightType GetType() const override;
};


// class SpotLight:public Light{
// public:
//     SpotLight();
// };