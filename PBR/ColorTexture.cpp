#include "pch.h"
#include "ColorTexture.h"

ColorTexture::ColorTexture(const glm::vec3& _color) 
	: color(_color)
{

}

ColorTexture::~ColorTexture() {

}

glm::vec3 ColorTexture::Sample(const glm::vec2& uv, const glm::vec3& p, float time) const {
	return color;
}