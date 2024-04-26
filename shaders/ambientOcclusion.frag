#version 450

// #extension GL_EXT_nonuniform_qualifier : enable

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantAO_T
{
    PushConstantAO pushConstant;
};

// Write to Attachment
layout(location = 0) out vec4 outColor;

// Descriptor Set Binding
layout(binding = 0) uniform sampler2D renderedImage;

layout(binding = 1) uniform sampler2D positionMap;

layout(binding = 2) uniform sampler2D normalMap;

layout(binding = 3) uniform sampler2D specularMap;

void main()
{
    outColor = vec4(0.0, 0.7, 1.0, 1.0);
}