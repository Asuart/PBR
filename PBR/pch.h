#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <utility>
#include <random>
#include <vector>
#include <array>
#include <queue>
#include <unordered_map>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define Float float
#define Infinity std::numeric_limits<float>::infinity()
#define PI 3.14159265359f
#define INV_PI 1.0f / PI
#define EPSILON 0.001f

const Float ShadowEpsilon = 0.0001f;

enum class TransportMode { Radiance, Importance };
enum class LightFlags : int {
	DeltaPosition = 1, DeltaDirection = 2, Area = 4, Infinite = 8
};

glm::vec3 UniformSampleSphere(const glm::vec2& u) {
	Float z = 1 - 2 * u[0];
	Float r = std::sqrt(std::max((Float)0, (Float)1 - z * z));
	Float phi = 2 * PI * u[1];
	return glm::vec3(r * std::cos(phi), r * std::sin(phi), z);
}

Float UniformSpherePdf() { return 1.0f / (PI * 4.0); }

class AssimpGLMHelpers {
public:
	static inline glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from) {
		glm::mat4 to;
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	static inline glm::vec3 GetGLMVec(const aiVector3D& vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::quat GetGLMQuat(const aiQuaternion& pOrientation) {
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};
