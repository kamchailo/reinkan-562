#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 viewInverse;
    mat4 proj;
    vec2 screenExtent;
} ubo;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPos;
layout(location = 1) out vec3 viewDir;
layout(location = 2) out vec3 vertexNormal;

void main() 
{
    mat4 modelTransform = ubo.proj * ubo.view * ubo.model;

    gl_Position =  modelTransform * vec4(inPosition, 1.0);

    vec3 eye = vec3(ubo.viewInverse * vec4(0, 0, 0, 1));

    // out
    worldPos =  (ubo.model * vec4(inPosition, 1.0)).xyz;
    viewDir = vec3(eye - worldPos);
}