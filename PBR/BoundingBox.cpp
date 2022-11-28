#include "pch.h"
#include "BoundingBox.h"

BoundingBox::BoundingBox(const glm::vec3& _pMin, const glm::vec3& _pMax) 
	: pMin(_pMin), pMax(_pMax)
{

}

BoundingBox::~BoundingBox() {

}
inline constexpr float gamma(int n) {
    constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

bool BoundingBox::IsIntersected(const Ray& ray, float* out_hit0, float* out_hit1) const {
    float t0 = 0, t1 = 10000.0;
    for (int i = 0; i < 3; ++i) {
        float invRayDir = 1 / ray.direction[i];
        float tNear = (pMin[i] - ray.origin[i]) * invRayDir;
        float tFar = (pMax[i] - ray.origin[i]) * invRayDir;
        if (tNear > tFar) std::swap(tNear, tFar);
        tFar *= 1 + 2 * gamma(3);

        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) return false;

    }
    if (out_hit0) *out_hit0 = t0;
    if (out_hit1) *out_hit1 = t1;
    return true;
}

bool BoundingBox::TestHit(const Ray& ray, float tMin, float tMax) const {
    for (int a = 0; a < 3; a++) {
        float invD = 1.0f / ray.direction[a];
        float t0 = (pMin[a] - ray.origin[a]) * invD;
        float t1 = (pMax[a] - ray.origin[a]) * invD;
        if (invD < 0.0f)
            std::swap(t0, t1);
        tMin = t0 > tMin ? t0 : tMin;
        tMax = t1 < tMax ? t1 : tMax;
        if (tMax <= tMin)
            return false;
    }
    return true;
}

glm::vec3 BoundingBox::Diagonal() const {
	return pMax - pMin;
}

float BoundingBox::SurfaceArea() const {
	glm::vec3 d = Diagonal();
	return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
}

float BoundingBox::Volume() const {
	glm::vec3 d = Diagonal();
	return d.x * d.y * d.z;
}

BoundingBox SurroundingBox(const BoundingBox& box0, const BoundingBox& box1) {
	glm::vec3 small(fmin(box0.pMin.x, box1.pMin.x),
		fmin(box0.pMin.y, box1.pMin.y),
		fmin(box0.pMin.z, box1.pMin.z));

	glm::vec3 big(fmax(box0.pMax.x, box1.pMax.x),
		fmax(box0.pMax.y, box1.pMax.y),
		fmax(box0.pMax.z, box1.pMax.z));

	return BoundingBox(small, big);
}