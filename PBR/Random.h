#pragma once
#include "pch.h"

inline float RandomFloat();
inline float RandomFloat(float min, float max);

inline glm::vec3 RandomVector();
inline glm::vec3 RandomVector(float min, float max);
inline glm::vec3 RandomUnitVector();

glm::vec3 RandomInUnitSphere();

glm::vec3 RandomInHemisphere(const glm::vec3& normal);

glm::vec3 RandomInUnitDisk();

glm::vec3 RandomCosineDirection();

class ONB {
public:
    ONB();

    inline glm::vec3 operator[](int32_t i) const;

    inline glm::vec3 u() const;
    inline glm::vec3 v() const;
    inline glm::vec3 w() const;

    glm::vec3 local(float a, float b, float c) const;

    glm::vec3 local(const glm::vec3& a) const;

    void build_from_w(const glm::vec3&);

public:
    glm::vec3 axis[3];
};