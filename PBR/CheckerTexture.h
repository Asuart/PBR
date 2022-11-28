#pragma once
#include "Textures.h"

class CheckerTexture : public Texture {
	std::shared_ptr<Texture> even, odd;
public:
	CheckerTexture(std::shared_ptr<Texture> _even, std::shared_ptr<Texture> _odd);
	CheckerTexture(const glm::vec3& color1, const glm::vec3& color2);
	~CheckerTexture();

	virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& p, float time = 0.0f) const override;
};

