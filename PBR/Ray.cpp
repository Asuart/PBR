#include "pch.h"
#include "Ray.h"

Ray::Ray(const glm::vec3& _origin, const glm::vec3& _direction, float _time)
	: origin(_origin), direction(_direction), time(_time)
{

}

Ray::~Ray() {

}

glm::vec3 Ray::At(float t) const {
	return origin + direction * t;
}