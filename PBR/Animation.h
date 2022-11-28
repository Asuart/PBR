#pragma once
#include "pch.h"
#include "Bone.h"
#include "SceneObject.h"

struct AssimpNodeData {
    glm::mat4 transformation;
    std::string name;
    int32_t childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation {
public:
    Animation() = default;
    Animation(const std::string& animationPath, SceneObject* model);
    ~Animation();

    Bone* FindBone(const std::string& name);

    float GetTicksPerSecond() const;
    float GetDuration() const;
    const AssimpNodeData& GetRootNode() const;
    const std::map<std::string, BoneInfo>& GetBoneIDMap() const;

private:
    float duration;
    int32_t ticksPerSecond;
    std::vector<Bone> bones;
    AssimpNodeData rootNode;
    std::map<std::string, BoneInfo> boneInfoMap;

    void ReadMissingBones(const aiAnimation* animation, SceneObject& model);
    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src);
};