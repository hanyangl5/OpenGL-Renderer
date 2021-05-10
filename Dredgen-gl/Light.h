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

private:
    glm::vec3 color;
    glm::vec3 pos, dir;
};

class DirectLight : public Light
{
    public:
    DirectLight(glm::vec3 c,glm::vec3 p,glm::vec3 d);
    glm::vec3 GetColor()const override;
};

// the point light dir is (0,0,0)
class PointLight : public Light
{
public:
    PointLight(glm::vec3 c, glm::vec3 p);
    glm::vec3 GetColor()const override;
};
