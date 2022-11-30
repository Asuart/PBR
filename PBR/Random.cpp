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
	return glm::normalize(RandomVector(-1.0, 1.0));
}

glm::vec3 RandomInUnitSphere() {
	return glm::normalize(RandomVector(-1.0, 1.0));
}

glm::vec3 RandomInHemisphere(const glm::vec3& normal) {
	glm::vec3 uSphere = RandomUnitVector();
	if (dot(uSphere, normal) > 0.0) // In the same hemisphere as the normal
		return uSphere;
	return -uSphere;
}

inline glm::vec3 RandomInUnitDisk() {
	RandomInUnitSphere();
	return glm::normalize(glm::vec3(RandomFloat(-1, 1), RandomFloat(-1, 1), 0));
}


glm::vec3 RandomCosineDirection() {
	float r1 = RandomFloat();
	float r2 = RandomFloat();
	float z = sqrt(1 - r2);

	float phi = 2 * PI * r1;
	float x = cos(phi) * sqrt(r2);
	float y = sin(phi) * sqrt(r2);

	return glm::vec3(x, y, z);
}


ONB::ONB() {}

inline glm::vec3 ONB::operator[](int32_t i) const { 
	return axis[i];
}

inline glm::vec3 ONB::u() const { 
	return axis[0]; 
}

inline glm::vec3 ONB::v() const { 
	return axis[1]; 
}

inline glm::vec3 ONB::w() const { 
	return axis[2]; 
}

glm::vec3 ONB::local(float a, float b, float c) const {
	return a * u() + b * v() + c * w();
}

glm::vec3 ONB::local(const glm::vec3& a) const {
	return a.x * u() + a.y * v() + a.z * w();
}

void ONB::build_from_w(const glm::vec3& n) {
	axis[2] = RandomInHemisphere(n);
	glm::vec3 a = (fabs(w().x) > 0.9) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
	axis[1] = RandomInHemisphere(cross(w(), a));
	axis[0] = cross(w(), v());
}