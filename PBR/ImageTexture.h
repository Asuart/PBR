#pragma once
#include "Texture.h"

class ImageTexture : public Texture {
public:
	ImageTexture();
	ImageTexture(const char* filePath);
	~ImageTexture();

	virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& p, float time = 0.0f) const override;

private:
	glm::vec3* data;
	int32_t width;
	int32_t height;
};

