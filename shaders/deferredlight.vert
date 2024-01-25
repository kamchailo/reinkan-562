#version 450

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantDeferredLight_T
{
    PushConstantDeferredLight pushConstant;
};

layout(binding = 0) uniform UniformBufferObject_T 
{
    UniformBufferObject ubo;
};

layout(std140, binding = 2) readonly buffer GlobalLightSSBO {
   LightObject globalLights[ ];
};  

layout(location = 0) in vec3    inPosition;
layout(location = 1) in vec3    inVertexNormal;

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec3 vertexNormal;
layout(location = 2) out int lightIndex;

void main()
{
    LightObject currentLight = globalLights[gl_InstanceIndex];

    // Scale the mesh to light radius
    mat4 modelTransform = mat4(currentLight.radius);
    modelTransform[3] = vec4(currentLight.position, 1.0);
    
    mat4 viewProjMatrix = ubo.proj * ubo.view * modelTransform;
    gl_Position = viewProjMatrix * vec4(inPosition, 1.0);;
    
    
    worldPos = (modelTransform * vec4(inPosition, 1.0)).xyz;
    vertexNormal = inVertexNormal;
    lightIndex = gl_InstanceIndex;
}