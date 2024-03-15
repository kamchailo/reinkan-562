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

void main()
{
    vec2 uv = gl_FragCoord.xy/pushConstant.screenExtent;
    // outColor = vec4(uv, 0, 1);
    vec4 colorPass = texture(renderedImage, uv);
    float shadow = colorPass.a;

    if((pushConstant.debugFlag & 0x1) >= 1)
    {
        outColor = vec4(texture(shadowMap, uv).rgb, 1);
        return;
    }

    if((pushConstant.debugFlag & 0x2) > 1)
    {
        outColor = colorPass;
        return;
    }
    if((pushConstant.debugFlag & 0x4) > 1)
    {
        outColor = vec4(texture(positionMap, uv).rgb, 1);
        return;
    }
    if((pushConstant.debugFlag & 0x8) > 1)
    {
        outColor = vec4(texture(normalMap, uv).rgb, 1);
        return;
    }
    if((pushConstant.debugFlag & 0x10) > 1)
    {
        outColor = vec4(texture(specularMap, uv).rgb, 1);
        return;
    }
    if((pushConstant.debugFlag & 0x20) > 1)
    {
        shadow = 1.0;
    }
    
    // Global light
    // vec3 albedo     = texture(renderedImage, uv).rgb;
    vec3 position   = texture(positionMap, uv).rgb;
    vec3 normal     = normalize(texture(normalMap, uv).rgb);
    vec3 specular   = texture(specularMap, uv).rgb;

    vec3 L = normalize(pushConstant.globalLightPosition.xyz - position);
    vec3 V = normalize(pushConstant.cameraPosition.xyz - position);

    Material pixelMaterial;
    pixelMaterial.diffuse = colorPass.rgb;
    pixelMaterial.specular = specular;
    pixelMaterial.shininess = pushConstant.debugFloat;

    vec3 brdfColor = EvalBrdf(normal, L, V, pixelMaterial) * 0.5;

    // add local lights
    vec3 finalColor = (colorPass * 0.1).rgb + shadow * brdfColor + texture(deferredImage, uv).rgb;

    outColor = vec4(finalColor, 1.0);

    if((pushConstant.debugFlag & 0x40) > 1)
    {
        vec3 lightShaft = texture(vlightMap, uv).rgb;
        vec3 dodgeLightShaft = finalColor * lightShaft;
        outColor = vec4(finalColor + dodgeLightShaft + (lightShaft * 0.03), 1);
        // outColor = vec4(finalColor + lightShaft, 1.0);
    }


}