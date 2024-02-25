#version 450

#extension GL_EXT_nonuniform_qualifier : enable
// #extension GL_EXT_buffer_reference2 : require

#include "SharedStruct.glsl"

layout(push_constant) uniform PushConstantRaster_T
{
    PushConstant pushConstant;
};

layout(binding = 0) uniform UniformBufferObject_T 
{
    UniformBufferObject ubo;
};

layout(binding = 1) buffer MaterialBlock 
{
    // Get material by using PushConstant::materialId
    // Material material = materials[pushConstant.materialId];
    Material materials[];
};

layout(binding = 2) uniform sampler2D[] textureSamplers;
// pack all textures together (diffuse, normal, height)
// Unpack by using Material::**texture**Id to Offset
// vec3 diffuse = textureSamplers[material.diffuseMapId];
// vec3 normal = textureSamplers[material.normalMapId];
// vec3 height = textureSamplers[material.heightMapId];

layout(std140, binding = 3) readonly buffer GlobalLightSSBO {
   LightObject globalLights[ ];
};  

layout(binding = 4) uniform sampler2D shadowmap;

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexTangent;
layout(location = 3) in vec3 vertexBitangent;
layout(location = 4) in vec3 viewDir;
layout(location = 5) in vec2 inFragTexCoord;
// Parallax
layout(location = 6) in vec3 TBNViewPos;
layout(location = 7) in vec3 TBNWorldPos;
layout(location = 8) in mat3 TBNMatrix;
// Shadow Map
layout(location = 11) in vec4 shadowCoord;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outSpecular;

// include BRDF calculation
#include "brdf.glsl"
#include "parallax.glsl"

uint tileNumberX = 16;
uint tileNumberY = 9;
uint tileNumberZ = 32;

void main() 
{
    Material material = materials[pushConstant.materialId];
    vec2 fragTexCoord = inFragTexCoord;

    ////////////////////////////////////////
    //          Shadow Mapping
    ////////////////////////////////////////
    int shadow = 0;
    vec2 shadowIndex = shadowCoord.xy/shadowCoord.w;
    float lightDepth = texture(shadowmap, shadowIndex).w;
    // minus constant to remove shadow acne
    float pixelDepth = shadowCoord.w - 0.01;

    if(shadowCoord.w > 0 && 
        shadowIndex.x >= 0 && shadowIndex.x <= 1 &&
        shadowIndex.y >= 0 && shadowIndex.y <= 1)
    {
        if(pixelDepth < lightDepth)
        {
            shadow = 1;
        }
    }

    if((pushConstant.debugFlag & 0x8) > 0)
    {
        shadow = 1;
    }

    if(material.diffuseMapId != -1)
    {
        vec3 diffuse = texture(textureSamplers[material.diffuseMapId], fragTexCoord).rgb;
        // overrride material
        material.diffuse = diffuse;
    }

    vec3 N = normalize(vertexNormal);
    vec3 normalMap = texture(textureSamplers[material.normalMapId], fragTexCoord).rgb;
    if(material.normalMapId <= 200)
    {
        normalMap = (normalMap * 2.0) - 1.0;
        N = normalize(TBNMatrix * normalMap);
        N.y = -N.y;
        outColor = vec4(1,0,0,1);
        return;
    }

    // Main Directional Light
    // vec3 L = normalize(ubo.globalLightPosition - worldPos);
    // float ambientLight = 0.06;
    // float intensity = 0.7;
    // vec3 V = normalize(-viewDir);
    // vec3 brdfColor = (ambientLight * material.diffuse) + shadow * intensity * EvalBrdf(N, L, V, material);
    
    // Final Color Result
    outColor = vec4(material.diffuse, 1.0);

    outPosition = vec4(worldPos, length(viewDir));

    outNormal = vec4(N, 1.0);

    outSpecular = vec4(material.specular, 1.0);

    ////////////////////////////////////////
    //          Debug Flag
    ////////////////////////////////////////
    if((pushConstant.debugFlag & 0x1) > 0)
    {

    }
}
