#include "pch.h"
#include "NoiseTexture.h"

NoiseTexture::NoiseTexture(glm::vec3 _scale) {
	scale = _scale;
}

NoiseTexture::~NoiseTexture() {

}

glm::vec3 NoiseTexture::Sample(const glm::vec2& uv, const glm::vec3& p, float time) const {
	return glm::vec3(0.5f * (1.0f + sin(scale * p.z + 10.0f * noise.Turbulence(p * scale))));
}

