#version 450

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantShadow_T
{
    PushConstantShadow pushConstant;
};

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 viewDir;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(worldPos, length(viewDir));
}