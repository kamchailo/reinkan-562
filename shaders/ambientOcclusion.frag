#version 450

// #extension GL_EXT_nonuniform_qualifier : enable

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantAO_T
{
    PushConstantAO pushConstant;
};

// Write to Attachment
layout(location = 0) out vec4 outColor;

// Descriptor Set Binding
layout(binding = 0) uniform sampler2D renderedImage;

layout(binding = 1) uniform sampler2D positionMap;

layout(binding = 2) uniform sampler2D normalMap;

layout(binding = 3) uniform sampler2D specularMap;

#define PI 3.14159265358979323846

void main()
{
    vec2 screenUV = gl_FragCoord.xy / pushConstant.screenExtent;
    vec3 position = texture(positionMap, screenUV).xyz;
    vec3 normal = texture(normalMap, screenUV).xyz;
    float depth = texture(positionMap, screenUV).a;

    float n = 20.0;
    float R = pushConstant.aoRange;
    float c = 0.1 * R;
    float delta = 0.001;

    int xHat = int(gl_FragCoord.x / PI) - 1;
    int yHat = int(gl_FragCoord.y);
    float phi = (30 * xHat ^ yHat) + 10 * xHat * yHat;

    float falloff = (2 * PI * c) / n;
    float integralValue = 0.0;
    for(int i = 0; i < n; ++i)
    {
        // Calculate the Spiral Points
        float alpha = (i + 0.5) / float(n);
        float h = (alpha * R / depth);
        float theta = 2 * PI * alpha * (7 * n / 9) + phi;
        
        // Get Pi the poaition at offset by spiral
        vec2 uvOffset = h * vec2(cos(theta), sin(theta));
        vec2 uvAtPi = screenUV + uvOffset;
        // If UVatPi is out of bound -> Inverse it
        if(uvAtPi.x > 1.0 || uvAtPi.x < 0.0)
        {
            uvAtPi.x = -uvAtPi.x;
        }
        if(uvAtPi.y > 1.0 || uvAtPi.y < 0.0)
        {
            uvAtPi.y = -uvAtPi.y;
        }
        vec3 Pi = texture(positionMap, uvAtPi).xyz;
        
        float depthAti = texture(positionMap, uvAtPi).a;
        
        // Direction to Pi from current Position
        vec3 Wi = Pi - position;
        
        float heaviside = 0;
        if(R - length(Wi) < 0.0)
        {
            heaviside = 0;
        }
        else
        {
            heaviside = 1;
        }
        
        float falloffAti = max(c * c, dot(Wi, Wi));

        integralValue += (max(0.0, dot(normal, Wi) - (delta * depthAti)) * heaviside)
                         / falloffAti;
    }    

    float S = falloff * integralValue;

    float scale = pushConstant.aoScale;
    float curveK = pushConstant.aoCurveK;

    float AO = max(0.0, (1 - scale * S) );

    outColor = vec4(vec3(AO), 1.0);
    return;
}