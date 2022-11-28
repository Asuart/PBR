#include "pch.h"
#include "Bone.h"

Bone::Bone(const std::string& name, int32_t ID, const aiNodeAnim* channel)
    : name(name), id(ID), localTransform(1.0f)
{
    numPositions = channel->mNumPositionKeys;
    for (int32_t positionIndex = 0; positionIndex < numPositions; positionIndex++) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
        data.timeStamp = timeStamp;
        positions.push_back(data);
    }

    numRotations = channel->mNumRotationKeys;
    for (int32_t rotationIndex = 0; rotationIndex < numRotations; rotationIndex++) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
        data.timeStamp = timeStamp;
        rotations.push_back(data);
    }

    numScalings = channel->mNumScalingKeys;
    for (int32_t keyIndex = 0; keyIndex < numScalings; keyIndex++) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.scale = AssimpGLMHelpers::GetGLMVec(scale);
        data.timeStamp = timeStamp;
        scales.push_back(data);
    }
}

void Bone::Update(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

glm::mat4 Bone::GetLocalTransform() const  { 
    return localTransform;
}

std::string Bone::GetBoneName() const { 
    return name; 
}

int32_t Bone::GetBoneID() const {
    return id; 
}

int32_t Bone::GetPositionIndex(float animationTime) const {
    for (int index = 0; index < numPositions - 1; index++) {
        if (animationTime < positions[index + 1].timeStamp)
            return index;
    }
}

int32_t Bone::GetRotationIndex(float animationTime) const {
    for (int index = 0; index < numRotations - 1; index++) {
        if (animationTime < rotations[index + 1].timeStamp)
            return index;
    }
}

int32_t Bone::GetScaleIndex(float animationTime) const {
    for (int index = 0; index < numScalings - 1; index++) {
        if (animationTime < scales[index + 1].timeStamp)
            return index;
    }
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    return midWayLength / framesDiff;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (numPositions == 1)
        return glm::translate(glm::mat4(1.0f), positions[0].position);

    int32_t p0Index = GetPositionIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (numRotations == 1) {
        auto rotation = glm::normalize(rotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int32_t p0Index = GetRotationIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(rotations[p0Index].timeStamp, rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime) {
    if (numScalings == 1)
        return glm::scale(glm::mat4(1.0f), scales[0].scale);

    int32_t p0Index = GetScaleIndex(animationTime);
    int32_t p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(scales[p0Index].timeStamp, scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}