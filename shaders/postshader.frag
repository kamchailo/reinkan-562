#version 450

#include "SharedStruct.glsl"
#include "vibrance.glsl"

layout(push_constant) uniform PushConstantRaster_T
{
    PushConstantPost pushConstant;
};

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D renderedImage;

layout(binding = 1) uniform sampler2D shadowMap;

layout(binding = 2) uniform sampler2D vlightMap;

layout(binding = 3) uniform sampler2D positionMap;

void main()
{
    vec2 uv = gl_FragCoord.xy/pushConstant.screenExtent;
    // outColor = vec4(uv, 0, 1);
    vec4 colorPass = vec4(texture(renderedImage, uv).rgb, 1);

    if((pushConstant.debugFlag & 0x2) > 1)
    {
        outColor = vec4(texture(shadowMap, uv).rgb, 1);
        // outColor = vec4(vec3(texture(shadowMap, uv).w) / 100, 1);
        return;
    }

    if((pushConstant.debugFlag & 0x4) > 1)
    {
        outColor = vec4(texture(vlightMap, uv).rgb, 1);
        return;
    }
    vec3 lightShaft = texture(vlightMap, uv).rgb * pushConstant.debugFloat2;

    vec3 dodgeLightShaft = colorPass.rgb * lightShaft;
    outColor = vec4(colorPass.rgb + dodgeLightShaft + (lightShaft * 0.3), 1);
}