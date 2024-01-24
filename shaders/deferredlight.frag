#version 450

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantDeferredLight_T
{
    PushConstantDeferredLight pushConstant;
};

layout(binding = 1) uniform sampler2D shadowMap;

layout(std140, binding = 2) readonly buffer GlobalLightSSBO {
   LightObject globalLights[ ];
};  

layout(binding = 3) uniform sampler2D renderedImage;

layout(binding = 4) uniform sampler2D positionMap;

layout(binding = 5) uniform sampler2D normalMap;

layout(binding = 6) uniform sampler2D specularMap;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(0.1, 0.0, 0.0, 1.0);
}