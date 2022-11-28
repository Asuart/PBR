#include "pch.h"
#include "Mesh.h"

Vertex::Vertex(const glm::vec3& _p, const glm::vec3& _n, const glm::vec2& _uv) 
    : p(_p), n(_n), uv(_uv) 
{
    for (int32_t i = 0; i < MAX_BONES_PER_VERTEX; i++) {
        boneIDs[i] = -1;
        boneWeights[i] = 0.0f;
    }
}

Mesh::Mesh(const std::vector<glm::vec3>& _positions, const std::vector<int32_t>& _indices)
    : indices(_indices)
{
    if (_positions.size() % 3) {
        std::cout << "Wrong number of vertices in mesh: " << _positions.size() << "\n";
        return;
    }

    for (int32_t i = 0; i < _positions.size(); i += 3) {
        glm::vec3 v0v1 = _positions[i + 1] - _positions[i];
        glm::vec3 v0v2 = _positions[i + 2] - _positions[i];
        glm::vec3 normal = glm::normalize(glm::cross(v0v1, v0v2));
        vertices.push_back(Vertex(_positions[i], normal));
        vertices.push_back(Vertex(_positions[i + 1], normal));
        vertices.push_back(Vertex(_positions[i + 2], normal));
    }

    if (_indices.size() == 0) {
        for (int32_t i = 0; i < vertices.size(); i++) {
            indices.push_back(i);
        }
    }

    UpdateWorldBounds();
}

Mesh::~Mesh() {}

bool Mesh::TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& out_collision) const {
    bool hitAny = false;
    //if (!worldBounds.IsIntersected(ray, nullptr, nullptr))
    //    return hitAny;
    for (int32_t i = 0; i < vertices.size(); i+=3) {
        const Vertex& v0 = vertices[i];
        const Vertex& v1 = vertices[i + 1];
        const Vertex& v2 = vertices[i + 2];

        const glm::vec3 normal = v0.n;

        float NdotRayDirection = glm::dot(normal, ray.direction);
        if (fabs(NdotRayDirection) < 0.001)
            continue;

        float d = -glm::dot(normal, v0.p);
        float t = -(glm::dot(normal, ray.origin) + d) / NdotRayDirection;
        if (t < tMin || t > tMax) continue;

        glm::vec3 P = ray.At(t);
        glm::vec3 C;

        glm::vec3 edge0 = v1.p - v0.p;
        glm::vec3 vp0 = P - v0.p;
        C = glm::cross(edge0, vp0);
        if (glm::dot(normal, C) < 0)
            continue;

        glm::vec3 edge1 = v2.p - v1.p;
        glm::vec3 vp1 = P - v1.p;
        C = glm::cross(edge1, vp1);
        if (glm::dot(normal, C) < 0)
            continue;

        glm::vec3 edge2 = v0.p - v2.p;
        glm::vec3 vp2 = P - v2.p;
        C = glm::cross(edge2, vp2);
        if (glm::dot(normal, C) < 0)
            continue;

        hitAny = true;
        tMax = t;
        out_collision.collided = true;
        out_collision.distance = t;
        out_collision.backFace = NdotRayDirection < 0;
        out_collision.materialIndex = materialIndex;
        out_collision.normal = !out_collision.backFace  ? -normal : normal;
        out_collision.position = P;
        out_collision.uv = glm::vec3(0);
    }

    return hitAny;
}

void Mesh::ApplyTransform(const Transform& _transform) {
    glm::mat4 mTransformPositions = _transform.GetTransformMatrix();
    glm::mat4 mRevertPositions = transform.GetInverseTransformMatrix();

    glm::mat4 mRevertNormals = glm::inverse(glm::transpose(transform.GetInverseTransformMatrix()));
    glm::mat4 mTransformNormals = glm::transpose(_transform.GetInverseTransformMatrix());

    for (int32_t i = 0; i < vertices.size(); i++) {
        glm::vec4 newP = mTransformPositions * mRevertPositions * glm::vec4(vertices[i].p, 1.0);
        vertices[i].p = glm::vec3(newP.x, newP.y, newP.z) / newP.w;
        glm::vec4 newN = mTransformNormals * mRevertNormals * glm::vec4(vertices[i].n, 1.0);
        vertices[i].n = glm::normalize(glm::vec3(newN.x, newN.y, newN.z));
    }    

    glm::vec4 newPmin = mTransformPositions * mRevertPositions * glm::vec4(worldBounds.pMin, 1.0);
    worldBounds.pMin = glm::vec3(newPmin.x, newPmin.y, newPmin.z) / newPmin.w;

    glm::vec4 newPmax = mTransformPositions * mRevertPositions * glm::vec4(worldBounds.pMax, 1.0);
    worldBounds.pMax = glm::vec3(newPmax.x, newPmax.y, newPmax.z) / newPmax.w;

    transform = _transform;
}

void Mesh::SetMaterial(int32_t _materialIndex) {
    materialIndex = _materialIndex;
}

const BoundingBox& Mesh::GetWorldBounds() const {
    return worldBounds;
}

void Mesh::UpdateWorldBounds() {
    glm::vec3 pMin(10000.0), pMax(-10000.0);
    for (int32_t i = 0; i < vertices.size(); i++) {
        if (vertices[i].p.x < pMin.x) pMin.x = vertices[i].p.x;
        if (vertices[i].p.y < pMin.y) pMin.y = vertices[i].p.y;
        if (vertices[i].p.z < pMin.z) pMin.z = vertices[i].p.z;
        if (vertices[i].p.x > pMax.x) pMax.x = vertices[i].p.x;
        if (vertices[i].p.y > pMax.y) pMax.y = vertices[i].p.y;
        if (vertices[i].p.z > pMax.z) pMax.z = vertices[i].p.z;
    }
    worldBounds = BoundingBox(pMin, pMax);
}

Mesh Mesh::CreateBox(const glm::vec3 size) {
    glm::vec3 hSize = size / 2.0f;

    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, -hSize.z));

    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, hSize.z));

    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, hSize.z));

    vertices.push_back(glm::vec3(-hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, -hSize.z));

    vertices.push_back(glm::vec3(hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(hSize.x, -hSize.y, hSize.z));

    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, -hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, hSize.y, hSize.z));
    vertices.push_back(glm::vec3(-hSize.x, -hSize.y, hSize.z));

    return Mesh(vertices);
}

