#pragma once
#include "pch.h"
#include "Animation.h"

class Animator {
public:
    Animator(Animation* Animation);

    void UpdateAnimation(float dt);
    void PlayAnimation(Animation* pAnimation);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
    std::vector<glm::mat4> GetFinalBoneMatrices();

private:
    std::vector<glm::mat4> finalBoneMatrices;
    Animation* currentAnimation;
    float currentTime;
    float deltaTime;
};