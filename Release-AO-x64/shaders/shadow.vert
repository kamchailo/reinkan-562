#version 450

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantShadow_T
{
    PushConstantShadow pushConstant;
};

layout(binding = 0) uniform ShadowUniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 viewInverse;
    mat4 proj;
    vec2 screenExtent;
} ubo;

layout(location = 0) in vec3    inPosition;
layout(location = 1) in vec3    inVertexNormal;
layout(location = 2) in vec3    inVertexTangent;
layout(location = 3) in vec3    inVertexBitangent;
layout(location = 4) in vec2    inTexCoord;

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec3 viewDir;

void main()
{
    mat4 modelTransform = ubo.proj * ubo.view * pushConstant.modelMatrix;
    
    gl_Position =  modelTransform * vec4(inPosition, 1.0);

    vec3 eye = vec3(ubo.viewInverse * vec4(0, 0, 0, 1));

    // out
    worldPos = vec3(pushConstant.modelMatrix * vec4(inPosition, 1.0));
    viewDir = vec3(eye - worldPos);
}