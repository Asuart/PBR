#include "pch.h"
#include "Sphere.h"

Sphere::Sphere(glm::vec3 _position, float _radius, uint32_t _materialIndex)
	: position(_position), radius(_radius), materialIndex(_materialIndex) {}

Sphere::~Sphere() {}

bool Sphere::TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& out_collision) const {
	glm::vec3 oc = ray.origin - position;
	float a = glm::length2(ray.direction);
	float half_b = glm::dot(oc, ray.direction);
	float c = glm::length2(oc) - radius * radius;

	float discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	float sqrtd = sqrt(discriminant);

	float root = (-half_b - sqrtd) / a;
	if (root < tMin || tMax < root) {
		root = (-half_b + sqrtd) / a;
		if (root < tMin || tMax < root)
			return false;
	}

	out_collision.collided = true;
	out_collision.distance = root;
	out_collision.position = ray.At(root);
	glm::vec3 outNormal = (out_collision.position - position) / radius;
	float theta = acos(-outNormal.y);
	float phi = atan2(-outNormal.z, outNormal.x) + PI;
	out_collision.uv = glm::vec2(phi / (2.0f * PI), theta / PI);
	bool front_face = glm::dot(ray.direction, outNormal) < 0;
	out_collision.normal = front_face ? outNormal : -outNormal;
	out_collision.materialIndex = materialIndex;
	out_collision.backFace = !front_face;

	return true;
}