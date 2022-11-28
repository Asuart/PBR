#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "Random.h"

class Texture {
public:
	glm::vec3 scale;
	virtual glm::vec3 Sample(const glm::vec2& uv, const glm::vec3& p, float time = 0.0f) const = 0;
};

