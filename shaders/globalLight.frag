#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#include "SharedStruct.glsl"
#include "brdf.glsl"

layout(push_constant) uniform PushConstantGlobalLight_T
{
    PushConstantGlobalLight pushConstant;
};

// Write to Attachment
layout(location = 0) out vec4 outColor;

// Descriptor Set Binding
layout(binding = 0) uniform sampler2D renderedImage;

layout(binding = 1) uniform sampler2D positionMap;

layout(binding = 2) uniform sampler2D normalMap;

layout(binding = 3) uniform sampler2D specularMap;

layout(binding = 4) uniform sampler2D shadowMap;

layout(binding = 5) uniform sampler2D[] hdrTextureSamplers;
// structure in pair
//  i * 2       = hdr
// (i * 2) + 1  = irr

layout(binding = 6) readonly buffer hammersleyNumberBlock
{
    //  i * 2       = u
    // (i * 2) + 1  = v
    float hammersleyUV[];
};

const uint HAMMERSLEY_NUMBER = 20;

void main()
{
    vec2 screenUV = gl_FragCoord.xy / pushConstant.screenExtent;
    vec3 normal = texture(normalMap, screenUV).xyz;
    vec3 Kd = texture(renderedImage,screenUV).xyz;

    uint hdrIndex = 2;

    float hammersleyNumber = pushConstant.distributionNumber * 2;
    for(int i = 0; i < hammersleyNumber; i+=2)
    {
        float u = float(i) / hammersleyNumber;
        float v = float(i + 1) / hammersleyNumber;
        if(length(screenUV - vec2(u,v)) <= 0.03)
        {
            outColor = vec4(1,0,1,1);
            return;
        }
    }

    vec2 uv;
    uv.x = 0.5 - (atan(normal.z, normal.x) / (2.0 * PI));
    uv.x = max(0.0, min(1.0, uv.x));
    uv.y = acos(normal.y) / PI;
    vec3 hdr = texture(hdrTextureSamplers[hdrIndex], uv).xyz;
    vec3 irradiance = texture(hdrTextureSamplers[hdrIndex + 1], uv).xyz * pushConstant.debugFloat2;
    outColor = vec4((Kd / PI) * (irradiance), 1.0);
}