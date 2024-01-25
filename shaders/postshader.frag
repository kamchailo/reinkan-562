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

layout(binding = 4) uniform sampler2D normalMap;

layout(binding = 5) uniform sampler2D specularMap;

layout(binding = 6) uniform sampler2D deferredImage;

void main()
{
    vec2 uv = gl_FragCoord.xy/pushConstant.screenExtent;
    // outColor = vec4(uv, 0, 1);
    vec4 colorPass = vec4(texture(renderedImage, uv).rgb, 1);

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
        if(outColor.r > 1.0 || outColor.g > 1.0 || outColor.b > 1.0)
        {
            outColor = vec4(1.0);
        }
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
    
    // outColor = vec4(0.1,0.1,0.7,1.0);

    outColor = (colorPass * 0.1) + vec4(texture(deferredImage, uv).rgb, 1);
    
    vec3 lightShaft = texture(vlightMap, uv).rgb * pushConstant.debugFloat2;

    vec3 dodgeLightShaft = colorPass.rgb * lightShaft;
    // outColor = vec4(colorPass.rgb + dodgeLightShaft + (lightShaft * 0.3), 1);

}