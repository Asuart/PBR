#pragma once
#include "Texture.h"

class ColorTexture : public Texture {
	glm::vec3 color;
public:
	ColorTexture(const glm::vec3& _color);
	~ColorTexture();

	virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& p, float time = 0.0f) const override;
};

