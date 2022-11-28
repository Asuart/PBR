#include "pch.h"
#include "CheckerTexture.h"

CheckerTexture::CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd)
	: even(_even), odd(_odd)
{

}

CheckerTexture::CheckerTexture(const glm::vec3& color1, const glm::vec3& color2)
	: even(std::make_shared<ColorTexture>(color1)), odd(std::make_shared<ColorTexture>(color2))
{

}

CheckerTexture::~CheckerTexture() {

}

glm::vec3 CheckerTexture::Sample(const glm::vec2& uv, const glm::vec3& p, float time) const {
    float sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
    if (sines < 0)
        return odd->Sample(uv, p, time);
    else
        return even->Sample(uv, p, time);
}