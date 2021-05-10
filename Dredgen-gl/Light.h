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
    virtual GetColor() const = 0;

private:
    glm::vec3 color;
    glm::vec3 pos, dir;
};

class DirectLight : public Light
{
    public:
    DirectLight();

};

// the point light dir is (0,0,0)
class PointLight : public Light
{
};
