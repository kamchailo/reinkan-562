#version 450

#include "SharedStruct.glsl"
#include "vibrance.glsl"
#include "brdf.glsl"

layout(push_constant) uniform PushConstantRaster_T
{
    PushConstantPost pushConstant;
};

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D renderedImage;

layout(binding = 1) uniform sampler2D shadowMap;

layout(binding = 2) uniform sampler2D vlightMap;

layout(binding = 3) uniform sampler2D positionMap;

layout(binding = 4) uniform sampler2D normalMap;

layout(binding = 5) uniform sampler2D specularMap;

layout(binding = 6) uniform sampler2D deferredImage;

layout(binding = 7) uniform sampler2D globalLightRenderedImage;

layout(binding = 8) uniform sampler2D blurShadow;

layout(binding = 9) uniform sampler2D ambientOcclusionMap;

layout(binding = 10) uniform sampler2D ambientOcclusionBlurMap;

void main()
{
    vec2 uv = gl_FragCoord.xy/pushConstant.screenExtent;
    vec4 colorPass = texture(globalLightRenderedImage, uv);
    float shadow = colorPass.a;
    float ambientOcclusion = texture(ambientOcclusionBlurMap, uv).r;

    if((pushConstant.debugFlag & 0x1) >= 1)
    {
        outColor = vec4(texture(shadowMap, uv).rgb, 1);
        return;
    }
    if((pushConstant.debugFlag & 0x2) > 1)
    {
        outColor = vec4(texture(ambientOcclusionMap, uv).rgb, 1);
        return;
    }
    if((pushConstant.debugFlag & 0x4) > 1)
    {
        outColor = vec4(texture(ambientOcclusionBlurMap, uv).rgb, 1);
        return;
    }
    if((pushConstant.debugFlag & 0x8) > 1)
    {
        outColor = vec4(texture(normalMap, uv).rgb, 1);
        return;
    }

    if((pushConstant.debugFlag & 0x20) > 1)
    {
        shadow = 1.0;
    }

    // Based Color from globalLightRenderedImage
    vec3 finalColor = (colorPass).rgb;

    // add local lights from deferredImage
    finalColor += texture(deferredImage, uv).rgb;

    // Add Ambient from ambientOcclusionMap
    finalColor *= ambientOcclusion + pushConstant.debugFloat2;

    outColor = vec4(finalColor, 1.0);

    /* Volumetric Light
    if((pushConstant.debugFlag & 0x40) > 1)
    {
        vec3 lightShaft = texture(vlightMap, uv).rgb;
        vec3 dodgeLightShaft = finalColor * lightShaft;
        outColor = vec4(finalColor + dodgeLightShaft + (lightShaft * 0.03), 1);
        // outColor = vec4(finalColor + lightShaft, 1.0);
    }
    */


}