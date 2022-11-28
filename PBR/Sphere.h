#include "SceneObject.h"

class Sphere {
public:
	Sphere(glm::vec3 _position, float _radius, uint32_t _materialIndex);
	~Sphere();

	bool TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& out_collision) const;
private:
	glm::vec3 position;
	float radius;
	uint32_t materialIndex;
};