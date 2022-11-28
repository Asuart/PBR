#pragma once
#include "pch.h"

struct BoneInfo {
	int32_t id;
	glm::mat4 offset;
};

struct KeyPosition {
	glm::vec3 position;
	float timeStamp;
};

struct KeyRotation {
	glm::quat orientation;
	float timeStamp;
};

struct KeyScale {
	glm::vec3 scale;
	float timeStamp;
};

class Bone {
public:
	Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel);

	void Update(float animationTime);

	glm::mat4 GetLocalTransform() const;
	std::string GetBoneName() const;
	int32_t GetBoneID() const;

	int32_t GetPositionIndex(float animationTime) const;
	int32_t GetRotationIndex(float animationTime) const;
	int32_t GetScaleIndex(float animationTime) const;

private:
	std::vector<KeyPosition> positions;
	std::vector<KeyRotation> rotations;
	std::vector<KeyScale> scales;
	int32_t numPositions;
	int32_t numRotations;
	int32_t numScalings;

	glm::mat4 localTransform;
	std::string name;
	int32_t id;

	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 InterpolatePosition(float animationTime);
	glm::mat4 InterpolateRotation(float animationTime);
	glm::mat4 InterpolateScaling(float animationTime);
};