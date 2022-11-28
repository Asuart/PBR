#include "pch.h"
#include "Random.h"

inline float RandomFloat() {
	return rand() / (RAND_MAX + 1.0);
}

inline float RandomFloat(float min, float max) {
	return min + (max - min) * RandomFloat();
}

inline glm::vec3 RandomVector() {
	return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
}

inline glm::vec3 RandomVector(float min, float max) {
	return glm::vec3(RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max));
}

inline glm::vec3 RandomUnitVector() {
	return glm::normalize(RandomVector());
}

glm::vec3 RandomInUnitSphere() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

glm::vec3 RandomInHemisphere(const glm::vec3& normal) {
	glm::vec3 uSphere = glm::normalize(RandomVector(-1.0, 1.0));
	if (dot(uSphere, normal) > 0.0) // In the same hemisphere as the normal
		return uSphere;
	return -uSphere;
}

inline glm::vec3 RandomInUnitDisk() {
	RandomInUnitSphere();
	return glm::normalize(glm::vec3(RandomFloat(-1, 1), RandomFloat(-1, 1), 0));
}