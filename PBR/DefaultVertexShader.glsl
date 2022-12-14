#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout(location = 2) in ivec4 boneIDs; 
layout(location = 3) in vec4 weights;

out vec3 fNormal;

uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    mat4 BoneTransform = finalBonesMatrices[boneIDs[0]] * weights[0];
    BoneTransform += finalBonesMatrices[boneIDs[1]] * weights[1];
    BoneTransform += finalBonesMatrices[boneIDs[2]] * weights[2];
    BoneTransform += finalBonesMatrices[boneIDs[3]] * weights[3];

    vec4 PosL = BoneTransform * vec4(pos, 1.0);
    gl_Position = mProjection * mView * PosL;
    //TexCoord0 = TexCoord;
    //vec4 NormalL = BoneTransform * vec4(norm, 0.0);
    //Normal0 = (gWorld * NormalL).xyz;
    //WorldPos0 = (gWorld * PosL).xyz;
    
    
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
        if(boneIDs[i] == -1) 
            continue;
        if(boneIDs[i] >= MAX_BONES){
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIDs[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIDs[i]]) * norm;
    }
    gl_Position = mProjection * mView * vec4(pos, 1.0);
    
    fNormal = norm;

    
}