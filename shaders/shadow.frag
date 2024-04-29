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

    // float minLength = 3.0;
    // float maxLength = 25.0;

    // float depth = length(viewDir);

    // depth = (depth - minLength) / (minLength + maxLength);

    // float depth2 = depth * depth;
    // float depth3 = depth2 * depth;
    // float depth4 = depth2 * depth2;

    // outColor = vec4 (depth, depth2, depth3, depth4);

}