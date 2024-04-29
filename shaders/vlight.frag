#version 450

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantVLight_T
{
    PushConstantVLight pushConstant;
};

layout(binding = 0) uniform UniformBufferObject_T 
{
    UniformBufferObject ubo;
};

layout(binding = 1) uniform sampler2D shadowmap;
layout(binding = 2) uniform sampler2D depthmap;

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in float lightOrigin;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 shaftColor = pushConstant.lightConstant;
    float shaftDistance = length(pushConstant.cameraPosition.xyz - worldPos);

    vec2 uv = gl_FragCoord.xy / ubo.screenExtent;

    float bias = 0.1;
    vec3 scenePos = texture(depthmap, uv).rgb;
    float sceneDepth = length(pushConstant.cameraPosition.xyz - scenePos);

    float cullDistance = min(shaftDistance, sceneDepth);

    vec3 landingPosition;
    if(shaftDistance < cullDistance)
    {
        landingPosition = worldPos;
    }
    else
    {
        landingPosition = scenePos;
    }

    // float distanceToLight = max(0.0, 10.0 - length(pushConstant.lightPosition.rgb - landingPosition));
    float distanceToLight = length(pushConstant.lightPosition.rgb - landingPosition) * pushConstant.lightDistanceScale;
    float radius = pushConstant.lightRadius;
    float brightness = (1 / (distanceToLight * distanceToLight)) - (1 / (radius * radius));


    int facing = 1;

    if(gl_FrontFacing)
    {
        facing = -1;
    }
    
    float scale = mix(0.5, brightness, pushConstant.debugFloat);

    // vec4 colorA = facing * cullDistance * shaftColor * scale;
    // vec4 colorB = facing * cullDistance * cullDistance * cullDistance * cullDistance * shaftColor * scale ;
    // vec4 mixColor = mix(colorA, colorB, pushConstant.debugFloat);
    outColor = facing * cullDistance * shaftColor * scale;
    // outColor = vec4(vec3(shaftDistance / 1000), 1.0);

}