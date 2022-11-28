#pragma once
#include "pch.h"

class CollisionInfo {
public:
	bool collided;
	bool backFace;
	int32_t materialIndex;
	float distance;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;

	CollisionInfo();
	~CollisionInfo();
};

