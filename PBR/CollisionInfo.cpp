#include "pch.h"
#include "CollisionInfo.h"

CollisionInfo::CollisionInfo()
	: collided(false), backFace(false), position(glm::vec3(0)), normal(glm::vec3(0)), uv(glm::vec2(0)), distance(10000.0), materialIndex(0)
{

}

CollisionInfo::~CollisionInfo() {

}