#pragma once
#include "Texture.h"
#include "PerlinNoise.h"

class NoiseTexture : public Texture {
	PerlinNoise noise;
public:
	NoiseTexture(glm::vec3 _scale = glm::vec3(1.0));
	~NoiseTexture();

	virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& p, float time = 0.0f) const override;
};

