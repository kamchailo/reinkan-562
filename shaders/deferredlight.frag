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

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in flat int lightIndex;

layout(location = 0) out vec4 outColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / pushConstant.screenExtent;

    vec3 albedo     = texture(renderedImage, uv).rgb;
    vec3 position   = texture(positionMap, uv).rgb;
    vec3 normal     = texture(normalMap, uv).rgb;
    vec3 specular   = texture(specularMap, uv).rgb;

    LightObject currentLight = globalLights[lightIndex];

    vec3 lightDirection = currentLight.position - position;

    float distToLight = length(lightDirection);

    if(distToLight > currentLight.radius)
    {
        discard;
    }

    float brightness = ((1 / (distToLight * distToLight)) - (1 / (currentLight.radius * currentLight.radius))) * currentLight.intensity;

    // brightness = max(0.0, brightness);

    vec3 lightColor = currentLight.color * brightness;

    vec3 L = normalize(lightDirection);

    float NL = max(dot(normal,L),0);

    outColor = vec4(vec3(lightColor * NL), 1.0);
    // outColor = vec4(vec3(lightIndex / 5), 1.0);
    // outColor = vec4(position, 1.0);
    // outColor = vec4(GetDebugIntColor(lightIndex), 1.0);
}