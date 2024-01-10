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

layout(std140, binding = 4) readonly buffer ClusterGridBlock 
{
    ClusterGrid clusterGrids[];
};

layout(std140, binding = 5) readonly buffer ClusterPlaneBlock 
{
    ClusterPlane clusterPlanes[];
};

// LightIndex
layout(std140, binding = 6) readonly buffer LightIndexSSBO {
   uint lightIndexList[ ];
};

layout(std140, binding = 7) readonly buffer LightGridSSBO {
   LightGrid lightGrids[ ];
};

layout(binding = 8) uniform sampler2D shadowmap;

// layout(binding = 9) uniform sampler2D[] pyramidalSamplers;

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

vec3 colorSample[8] = {vec3(1.0, 0.0, 0.0), 
                        vec3(1.0, 1.0, 0.0), 
                        vec3(0.0, 1.0, 0.0), 
                        vec3(0.0, 1.0, 1.0), 
                        vec3(0.0, 0.0, 1.0), 
                        vec3(1.0, 0.0, 1.0), 
                        vec3(0.0, 0.0, 0.0), 
                        vec3(1.0, 1.0, 1.0)}; 

float LinearDepth(float depthSample, float zNear, float zFar)
{
    float linear = zNear * zFar / (zFar + zNear - depthSample * (zFar - zNear));
    return linear;
}

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
    }

    // Main Directional Light
    vec3 L = normalize(ubo.globalLightPosition - worldPos);
    float ambientLight = 0.06;
    float intensity = 0.7;
    vec3 V = normalize(-viewDir);
    vec3 brdfColor = (ambientLight * material.diffuse) + shadow * intensity * EvalBrdf(N, L, V, material);
    
    ////////////////////////////////////////
    //          Grid Calculation
    ////////////////////////////////////////
    // Define necessary variables
    float zNear = clusterPlanes[0].zNear;
    float zFar = clusterPlanes[tileNumberZ - 1].zFar;
    float linear = LinearDepth(gl_FragCoord.z, zNear, zFar);
    float aTerm = float(tileNumberZ) / log(zFar/ zNear);
    // Z Plane of current Grid
    uint sliceFlat = uint(log(linear) * (aTerm) - aTerm * log(zNear));

    uint tileSizeX = uint(ubo.screenExtent.x / tileNumberX);
    uint tileSizeY = uint(ubo.screenExtent.y / tileNumberY);
    
    uvec3 tiles = uvec3( gl_FragCoord.x / tileSizeX, 
                         gl_FragCoord.y /tileSizeY, 
                         sliceFlat );

    uint tileIndex = tiles.x +
                     tileNumberX * tiles.y +
                     (tileNumberX * tileNumberY) * tiles.z;

    LightGrid lightGrid = lightGrids[tileIndex];
    uint offset = lightGrid.offset;

    // Loop through all light assigned in the grid
    for(int i = 0; i < lightGrid.size; ++i)
    {
        uint lightIndex = lightIndexList[offset + i];
        LightObject light = globalLights[lightIndex];

        float lightDistance = distance(light.position, worldPos);
        if(lightDistance >= light.radius)
        {
            continue;
        }
        L = normalize(light.position - worldPos);

        if((pushConstant.debugFlag & 0x4) > 1)
        {
            float intensity = light.intensity;
            brdfColor += intensity * 0.2 * light.color;
        }
        else
        {
            float intensity = light.intensity * (1 - lightDistance / light.radius);
            brdfColor += intensity * light.color * EvalBrdf(N, L, V, material);
        }
    }
    
    // Final Color Result
    outColor = vec4(brdfColor, 1.0);

    outPosition = vec4(worldPos, length(viewDir));

    outNormal = vec4(N, 1.0);

    outSpecular = vec4(0.5, 0.5, 0.5, 1.0);

    ////////////////////////////////////////
    //          Debug Flag
    ////////////////////////////////////////
    if((pushConstant.debugFlag & 0x1) > 0)
    {

    }
}
