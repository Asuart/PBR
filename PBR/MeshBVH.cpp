#include "pch.h"
#include "MeshBVH.h"

bool CachedTriangle::TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const {
    float NdotRayDirection = glm::dot(normal, ray.direction);
    if (fabs(NdotRayDirection) < 0.001)
        return false;

    float t = (d - glm::dot(normal, ray.origin)) / NdotRayDirection;
    if (t < tMin || t > tMax)
        return false;

    glm::vec3 P = ray.At(t);
    glm::vec3 C;

    glm::vec3 vp0 = P - v0;
    C = glm::cross(edge0, vp0);
    if (glm::dot(normal, C) < 0)
        return false;

    glm::vec3 vp1 = P - v1;
    C = glm::cross(edge1, vp1);
    if (glm::dot(normal, C) < 0)
        return false;

    glm::vec3 vp2 = P - v2;
    C = glm::cross(edge2, vp2);
    if (glm::dot(normal, C) < 0)
        return false;

    outCollision.collided = true;
    outCollision.distance = t;
    outCollision.backFace = NdotRayDirection < 0;
    outCollision.normal = outCollision.backFace ? normal : -normal;
    outCollision.position = P;
    outCollision.uv = glm::vec3(0);
    outCollision.area = area;

    return true;
}

MeshBVH::MeshBVH(std::vector<CachedTriangle>& _triangles, int32_t axisIndex, int32_t start, int32_t end) {
    if (end == -1) end = _triangles.size();
    int32_t trianglesCount = end - start;

    if (trianglesCount < maxTriangles) {
        BoundingBox nodeBounds(glm::vec3(10000.0f), glm::vec3(-10000.0f));
        for (uint32_t i = start; i < end; i++) {
            triangles.push_back(_triangles[i]);
            nodeBounds = SurroundingBox(nodeBounds, _triangles[i].bounds);
        }
        bounds = nodeBounds;
        return;
    }

    std::sort(_triangles.begin() + start, _triangles.begin() + end,
        [axisIndex](CachedTriangle const& a, CachedTriangle const& b) {
            return a.bounds.pMin[axisIndex] < b.bounds.pMin[axisIndex];
        });

    int32_t center = (start + end) / 2;

    children.push_back(new MeshBVH(_triangles, (axisIndex + 1) % 3, start, center));
    children.push_back(new MeshBVH(_triangles, (axisIndex + 1) % 3, center, end));
    bounds = SurroundingBox(children[0]->bounds, children[1]->bounds);
}

MeshBVH::~MeshBVH() {
    for (uint32_t i = 0; i < children.size(); i++) {
        delete children[i];
    }
}

bool MeshBVH::CheckCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& outCollision) const {
    if (!bounds.IsIntersected(ray, nullptr, nullptr))
        return false;

    bool hitAny = false;

    if (triangles.size() > 0) {
        for (uint32_t i = 0; i < triangles.size(); i++) {
            if (triangles[i].TestCollision(ray, tMin, tMax, outCollision)) {
                tMax = outCollision.distance;
                hitAny = true;
            }
        }
        return hitAny;
    }

    for (uint32_t i = 0; i < children.size(); i++) {
        if (children[i]->CheckCollision(ray, tMin, tMax, outCollision)) {
            tMax = outCollision.distance;
            hitAny = true;
        }
    }

    return hitAny;
}