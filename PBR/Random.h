#pragma once
#include "pch.h"

inline float RandomFloat();
inline float RandomFloat(float min, float max);

inline glm::vec3 RandomVector();
inline glm::vec3 RandomVector(float min, float max);
//inline glm::vec3 RandomUnitVector();

glm::vec3 RandomInUnitSphere();

glm::vec3 RandomInHemisphere(const glm::vec3& normal);

inline glm::vec3 RandomInUnitDisk();