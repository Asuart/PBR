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

BoundingBox CachedTriangle::GetBounds() const {
    float minX = glm::min(v0.x, glm::min(v1.x, v2.x));
    float minY = glm::min(v0.y, glm::min(v1.y, v2.y));
    float minZ = glm::min(v0.z, glm::min(v1.z, v2.z));
    float maxX = glm::max(v0.x, glm::max(v1.x, v2.x));
    float maxY = glm::max(v0.y, glm::max(v1.y, v2.y));
    float maxZ = glm::max(v0.z, glm::max(v1.z, v2.z));
    return BoundingBox(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
}

MeshBVH::MeshBVH(std::vector<CachedTriangle> _triangles, int32_t axisIndex)
    : leaf(false)
{
    BoundingBox box(glm::vec3(10000.0f), glm::vec3(-10000.0f));
    for (int32_t i = 0; i < _triangles.size(); i++) {
        BoundingBox objectBox = _triangles[i].GetBounds();
        box = SurroundingBox(box, objectBox);
    }
    bounds = box;

    if (_triangles.size() < maxTriangles) {
        leaf = true;
        for (uint32_t i = 0; i < _triangles.size(); i++) {
            triangles.push_back(_triangles[i]);
        }
        return;
    }

    std::sort(_triangles.begin(), _triangles.end(),
        [axisIndex](CachedTriangle const& a, CachedTriangle const& b) {
            BoundingBox box1 = a.GetBounds();
            BoundingBox box2 = b.GetBounds();
            return box1.pMin[axisIndex] < box2.pMin[axisIndex];
        });

    std::vector<CachedTriangle> left, right;
    for (uint32_t i = 0; i < _triangles.size() / 2; i++) {
        left.push_back(_triangles[i]);
    }
    for (uint32_t i = _triangles.size() / 2; i < _triangles.size(); i++) {
        right.push_back(_triangles[i]);
    }

    children.push_back(new MeshBVH(left, (axisIndex + 1) % 3));
    children.push_back(new MeshBVH(right, (axisIndex + 1) % 3));
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

    if (leaf) {
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

Mesh::Mesh(const std::vector<glm::vec3>& _positions, const std::vector<int32_t>& _indices)
    : indices(_indices), bvhRoot(nullptr)
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
    area = Area();
    UpdateCachedTriangles();
}

Mesh::~Mesh() {}

bool Mesh::TestCollision(const Ray& ray, float tMin, float tMax, CollisionInfo& out_collision) const {
    if (bvhRoot->CheckCollision(ray, tMin, tMax, out_collision)) {
        out_collision.materialIndex = materialIndex;
        return true;
    }
    return false;

    bool hitAny = false;
    //if (!worldBounds.IsIntersected(ray, nullptr, nullptr))
    //    return hitAny;
    for (int32_t i = 0; i < triangles.size(); i++) {
        if (triangles[i].TestCollision(ray, tMin, tMax, out_collision)) {
            hitAny = true;
            out_collision.materialIndex = materialIndex;
            tMax = out_collision.distance;
        }
    }
    return hitAny;
}


float Mesh::Area() {
    float area = 0.0f;
    for (int32_t i = 0; i < vertices.size(); i += 3) {
        const Vertex& v0 = vertices[i];
        const Vertex& v1 = vertices[i + 1];
        const Vertex& v2 = vertices[i + 2];
        area += glm::length(glm::cross(vertices[i + 1].p - vertices[i].p, vertices[i + 2].p - vertices[i].p)) / 2.0;
    }
    return area;
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

    transform = _transform;

    area = Area();
    UpdateWorldBounds();
    UpdateCachedTriangles();

    //std::cout << bvhRoot->bounds.pMin.x << " " << bvhRoot->bounds.pMin.y << " " << bvhRoot->bounds.pMin.z << "\n";
    //std::cout << bvhRoot->bounds.pMax.x << " " << bvhRoot->bounds.pMax.y << " " << bvhRoot->bounds.pMax.z << "\n\n";

}

void Mesh::SetMaterial(int32_t _materialIndex) {
    materialIndex = _materialIndex;
}

const BoundingBox& Mesh::GetWorldBounds() const {
    return worldBounds;
}


glm::vec3 Mesh::RandomInnerPoint() const {
    return glm::vec3(RandomFloat(worldBounds.pMin.x, worldBounds.pMax.x), RandomFloat(worldBounds.pMin.y, worldBounds.pMax.y), RandomFloat(worldBounds.pMin.z, worldBounds.pMax.z));
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

void Mesh::UpdateCachedTriangles() {
    triangles.clear();
    for (int32_t i = 0; i < indices.size(); i += 3) {
        CachedTriangle tri;
        Vertex v0 = vertices[indices[i]];
        Vertex v1 = vertices[indices[i + 1]];
        Vertex v2 = vertices[indices[i + 2]];

        tri.v0 = v0.p;
        tri.v1 = v1.p;
        tri.v2 = v2.p;
        tri.normal = glm::normalize(v0.n);
        tri.edge0 = v1.p - v0.p;
        tri.edge1 = v2.p - v1.p;
        tri.edge2 = v0.p - v2.p;
        tri.area = glm::length(glm::cross(vertices[i + 1].p - vertices[i].p, vertices[i + 2].p - vertices[i].p)) / 2.0;
        tri.d = glm::dot(tri.normal, v0.p);
    
        triangles.push_back(tri);
    }

    bvhRoot = std::make_shared<MeshBVH>(triangles);
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

