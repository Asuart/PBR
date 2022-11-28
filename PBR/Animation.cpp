#include "pch.h"
#include "Animation.h"

Animation::Animation(const std::string& animationPath, SceneObject* model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    ReadHeirarchyData(rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);
}

Animation::~Animation() {}

Bone* Animation::FindBone(const std::string& name) {
    auto iter = std::find_if(bones.begin(), bones.end(),
        [&](const Bone& Bone) {
            return Bone.GetBoneName() == name;
        }
    );
    if (iter == bones.end()) 
        return nullptr;
    else 
        return &(*iter);
}


float Animation::GetTicksPerSecond() const {
    return ticksPerSecond;
}

float Animation::GetDuration() const {
    return duration;
}

const AssimpNodeData& Animation::GetRootNode() const {
    return rootNode;
}

const std::map<std::string, BoneInfo>& Animation::GetBoneIDMap() const {
    return boneInfoMap;
}


void Animation::ReadMissingBones(const aiAnimation* animation, SceneObject& model) {
    int32_t size = animation->mNumChannels;

    auto& newBoneInfoMap = model.GetBoneInfoMap();
    int32_t& boneCount = model.GetBoneCount();

    for (int32_t i = 0; i < size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (newBoneInfoMap.find(boneName) == newBoneInfoMap.end()) {
            newBoneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        bones.push_back(Bone(channel->mNodeName.data,
            newBoneInfoMap[channel->mNodeName.data].id, channel));
    }

    boneInfoMap = newBoneInfoMap;
}

void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (int32_t i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        ReadHeirarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
    