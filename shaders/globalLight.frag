#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#include "SharedStruct.glsl"
#include "hammersley.glsl"

layout(push_constant) uniform PushConstantGlobalLight_T
{
    PushConstantGlobalLight pushConstant;
};

// Write to Attachment
layout(location = 0) out vec4 outColor;

// Descriptor Set Binding
layout(binding = 0) uniform sampler2D renderedImage;

layout(binding = 1) uniform sampler2D positionMap;

layout(binding = 2) uniform sampler2D normalMap;

layout(binding = 3) uniform sampler2D specularMap;

layout(binding = 4) uniform sampler2D shadowMap;

layout(binding = 5) uniform sampler2D[] hdrTextureSamplers;
// structure in pair
//  i * 2       = hdr
// (i * 2) + 1  = irr

layout(binding = 6) readonly buffer hammersleyNumberBlock
{
    //  i * 2       = u
    // (i * 2) + 1  = v
    float hammersleyUV[];
};

float G1(vec3 w, vec3 m, vec3 N, float alpha_suqared);
vec3 calculateHDRBRDF(vec3 Wk, vec3 V, vec3 N, vec3 Li, Material material);

const float PI = 3.14159265358979323846;

const uint HAMMERSLEY_NUMBER = 20;

const uint HDR_WIDTH = 2048;
const uint HDR_HEIGHT = 1024;

void main()
{
    uint hdrIndex = 2;
    vec2 screenUV = gl_FragCoord.xy / pushConstant.screenExtent;
    vec3 position = texture(positionMap, screenUV).xyz;
    vec3 normal = texture(normalMap, screenUV).xyz;
    float depth = texture(positionMap, screenUV).a;
    
    // SkySphere
    if(depth > 200.0)
    {
        vec3 viewDir = -normalize(pushConstant.cameraPosition.xyz - position);

        vec2 skyUV;
        skyUV.x = 0.5 - (atan(-viewDir.z, viewDir.x) / (2.0 * PI));
        skyUV.x = max(0.0, min(1.0, skyUV.x));
        skyUV.y = acos(viewDir.y) / PI;
        vec3 hdr = texture(hdrTextureSamplers[hdrIndex], skyUV).xyz;
        outColor = vec4(hdr, 1);
        return;
    }

    vec3 Kd = texture(renderedImage,screenUV).rgb;
    vec3 Ks = texture(specularMap, screenUV).rgb;
    float alpha = texture(specularMap, screenUV).a;

    vec2 uv;
    uv.x = 0.5 - (atan(normal.z, normal.x) / (2.0 * PI));
    uv.x = max(0.0, min(1.0, uv.x));
    uv.y = acos(normal.y) / PI;
    vec3 hdr = texture(hdrTextureSamplers[hdrIndex], uv).xyz;
    vec3 irradiance = texture(hdrTextureSamplers[hdrIndex + 1], uv).xyz * pushConstant.debugFloat;
    
    Material material;
    material.diffuse = 1.0 - Ks;
    material.specular = Ks;
    alpha = max(alpha, 0.001);
    material.shininess = alpha;

    vec3 N = normal;
    vec3 V = normalize(pushConstant.cameraPosition.xyz - position);

    vec3 sumBrdf = vec3(0);

    float NV = max(0.0, dot(normal,V));
    vec3 R = 2.0 * NV * normal - V;
    vec3 A = normalize(vec3(-R.y, R.x, 0.0));
    vec3 B = normalize(cross(R, A));

    for(int i = 0; i < HAMMERSLEY_NUMBER; ++i)
    {   
        vec2 hammersleyUV = hammersleySequence[i];

        float theta = atan((alpha * sqrt(hammersleyUV.y)) / sqrt(1.0 - hammersleyUV.y));
        hammersleyUV = vec2(hammersleyUV.x, theta / PI);

        float twoPi_HalfU = 2 * PI * (0.5 - hammersleyUV.x);
        float piV = PI * hammersleyUV.y;
        float sinPiV = sin(piV);
        vec3 L = vec3(cos(twoPi_HalfU) * sinPiV, 
                      sin(twoPi_HalfU) * sinPiV, 
                      cos(piV));
        L = normalize(L);

        vec3 Wk = normalize(L.x * A + L.y * B + L.z * R);

        // D factor GGX
        vec3 H = normalize(Wk + V);
        float mN = dot(H, N);
        float tan_square_theta_m = (1.0 - mN * mN) / (mN * mN);
        float alpha_square = alpha * alpha;
        float D = clamp(mN, 0.0, 1.0) * alpha_square
                    / (PI * pow(mN, 4) * pow(alpha_square + tan_square_theta_m, 2));


        // Level
        float level = (0.5 * log2(HDR_WIDTH * HDR_HEIGHT / HAMMERSLEY_NUMBER)) - (0.25 * log2(D)) - 1;
        level = max(0.0, level);

        uv.x = 0.5 - (atan(-Wk.z, Wk.x) / (2.0 * PI));
        uv.x = max(0.0, min(1.0, uv.x));
        uv.y = acos(Wk.y) / PI;
        vec3 Li = textureLod(hdrTextureSamplers[hdrIndex], uv, level).xyz;
        
        sumBrdf += calculateHDRBRDF(Wk, V, N, Li, material);

    }
    // Divided by total number of sample to get an average
    sumBrdf = sumBrdf / float(HAMMERSLEY_NUMBER) * pushConstant.debugFloat2;

    vec3 brdfColor = (Kd / PI) * (irradiance) + sumBrdf;
    
    // tone mapping to sRGB
    const float e = 0.6;
    vec3 finalColor = pow((e * brdfColor) / (e * brdfColor + vec3(1.0)), vec3(1.0/2.2));

    outColor = vec4(finalColor, 1.0);
}

float G1(vec3 w, vec3 m, vec3 N, float alpha_suqared)
{
    float wN = dot(w, N);
    float wm = dot(w, m);
    float tan_square_theta_w = (1.0 - wN * wN) / (wN * wN);
    float G1;
    if(wN > 1.0 || sqrt(tan_square_theta_w) == 0) 
    {
        G1 = 1.0;
    } 
    else 
    {
        G1 = clamp(wm / wN, 0.0, 1.0) * (2 / (1 + sqrt(1 + alpha_suqared * tan_square_theta_w)));
    }
    return G1;
}

vec3 calculateHDRBRDF(vec3 Wk, vec3 V, vec3 N, vec3 Li, Material material)
{
    // Wk is L
    // vec3 Kd = material.diffuse;
    vec3 Ks = material.specular;
    const float alpha = material.shininess;
    

    // G Term
    vec3 H = normalize(Wk + V);
    float alpha_square = alpha * alpha;
    float G = G1(V, H, N, alpha_square) * G1(Wk, H, N, alpha_square);


    // F Term
    float LH = max(0.0, dot(Wk, H));
    vec3 F = Ks + (vec3(1.0) - Ks) * pow((1 - LH), 5);

    float NV = max(0.0, dot(N, V));
    float WkN = max(0.0, dot(Wk, N));

    // cosTheta or N . Wk (Omega_k)
    float NL = max(0.0, dot(N, Wk));

    return (G * F * Li * NL) 
            / 4.0 * (WkN) * (NV);
}
