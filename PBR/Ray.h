#pragma once
#include "pch.h"

struct Ray {
	glm::vec3 origin;
	glm::vec3 direction;
	float time;

	Ray(const glm::vec3& _origin = glm::vec3(0.0), const glm::vec3& _direction = glm::vec3(0.0, 0.0, 1.0), float time = 0.0f);
	~Ray();

	glm::vec3 At(float t) const;
};