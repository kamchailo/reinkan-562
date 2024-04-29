#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#include "SharedStruct.glsl"

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

layout(binding = 11) uniform sampler2D[] specialFunctionTextureSamplers;

vec2 CalculateAirlight(float beta, float intensity, vec3 sourcePosition, vec3 viewPosition, vec3 surfacePosition, sampler2D specialFunctionSampler);

void main()
{
    vec2 uv = gl_FragCoord.xy/pushConstant.screenExtent;
    vec4 colorPass = texture(globalLightRenderedImage, uv);
    float shadow = colorPass.a;
    float ambientOcclusion = texture(ambientOcclusionMap, uv).r;
    vec3 surfacePosition = texture(positionMap, uv).rgb;

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
        outColor = vec4(texture(vlightMap, uv).rgb, 1);
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
    
    vec2 airlight = CalculateAirlight(pushConstant.fogBeta, 
                                      pushConstant.lightIntensity, 
                                      pushConstant.globalLightPosition.rgb, 
                                      pushConstant.cameraPosition.rgb, 
                                      surfacePosition, 
                                      specialFunctionTextureSamplers[0]);

    // Based Color from globalLightRenderedImage
    vec3 finalColor = (colorPass).rgb;

    // add local lights from deferredImage
    finalColor += texture(deferredImage, uv).rgb;

    // Add Ambient from ambientOcclusionMap
    finalColor *= clamp(0.0, 1.0, ambientOcclusion);

    // Volumetric Light
    vec3 lightShaft = max(vec3(0.0),texture(vlightMap, uv).rgb);
    if((pushConstant.debugFlag & 0x40) > 1)
    {
        finalColor += lightShaft * pushConstant.debugFloat2;
    }

    // Fog
    float depth = texture(positionMap, uv).a;
    float fogThickness = depth / (pushConstant.fogFalloff * pushConstant.fogFalloff);
    finalColor.r += min(pushConstant.maxFogThickness, 0.20 * fogThickness);
    finalColor.g += min(pushConstant.maxFogThickness, 0.17 * fogThickness);
    finalColor.b += min(pushConstant.maxFogThickness, 0.05 * fogThickness);

    finalColor += min(fogThickness, pushConstant.maxFogThickness);
    finalColor += fogThickness * airlight.y;
    finalColor += airlight.x * vec3(1, 0.9, 0.5);
    outColor = vec4(finalColor, 1.0);
}

vec2 CalculateAirlight(float beta, 
                        float intensity, 
                        vec3 sourcePosition, 
                        vec3 viewPosition, 
                        vec3 surfacePosition, 
                        sampler2D specialFunctionSampler)
{
    const float PI = 3.1415926535;
    float Dsv = length(viewPosition - sourcePosition);
    float Tsv = beta * Dsv;
    float Dpv = length(viewPosition - surfacePosition); // It is Dvp on the paper
    float Tpv = beta * Dpv;
    vec3 Vsv = normalize(viewPosition - sourcePosition);
    vec3 Vpv = normalize(viewPosition - surfacePosition);
    float cosGamma = max(0.0, dot(Vsv, Vpv));
    float sinGamma = sqrt(1.0 - (cosGamma * cosGamma));

    float A1 = Tsv * sinGamma;

    float airlightSpot = 0.0;
    float scatter = 0.0;
    scatter = max(0.0, (1.0 - gamma));
    if(!(Dsv > Dpv))
    {
        airlightSpot = (max(0.0, (1.0 - A1)));
    }

    return vec2(airlightSpot, scatter);
}