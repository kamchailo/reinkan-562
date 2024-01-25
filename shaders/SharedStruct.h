#pragma once

#include <glm/glm.hpp>

namespace Reinkan
{
	constexpr uint32_t MAX_BONE_INFLUENCE{ 4 };

	struct Vertex
	{
		glm::vec3	position;
		glm::vec3	vertexNormal;
		glm::vec3	vertexTangent;
		glm::vec3	vertexBitangent;
		glm::vec2	texCoord;
	};

	struct UniformBufferObject 
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 viewInverse;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 shadowProjectionViewMatrix;
		alignas(16) glm::vec3 globalLightPosition;
		alignas(8) glm::vec2 screenExtent;
	};

	struct PushConstantScanline
	{
		alignas(16) glm::mat4 modelMatrix;
		alignas(16) glm::mat4 normalMatrix;
		alignas(4) uint32_t objectId;
		alignas(4) uint32_t materialId;
		alignas(4) uint32_t debugFlag;
		alignas(4) float debugFloat;
		alignas(4) float debugFloat2;
		alignas(4) int debugInt;
	};

	struct ShadowUniformBufferObject {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 viewInverse;
		alignas(16) glm::mat4 proj;
		alignas(8) glm::vec2 screenExtent;
	};

	struct PushConstantShadow
	{
		alignas(16) glm::mat4 modelMatrix;
		alignas(8)  glm::vec2 screenExtent;
	};

	// Use ScanlineUBO

	struct PushConstantVLight
	{
		alignas(16) glm::vec4 lightPosition;
		alignas(16) glm::vec4 lightConstant;
		alignas(16) glm::vec4 cameraPosition;
		alignas(8) glm::vec2 shadowMapExtent;
	};

	struct PushConstantDeferredLight
	{
		alignas(16) glm::vec4 DeferredLightConstant;
		alignas(16) glm::vec4 cameraPosition;
		alignas(8) glm::vec2 shadowMapExtent;
		alignas(8) glm::vec2 screenExtent;
	};

	struct PushConstantPost
	{
		alignas(8) glm::vec2 screenExtent;
		alignas(4) uint32_t debugFlag;
		alignas(4) float debugFloat;
		alignas(4) float debugFloat2;
		alignas(4) float debugFloat3;
	};

	struct Material {
		alignas(16) glm::vec3 diffuse;
		alignas(16) glm::vec3 specular;
		alignas(4) uint32_t normalMapId;
		alignas(4) uint32_t heightMapId;
		alignas(4) float shininess;
		alignas(4) uint32_t diffuseMapId;
		alignas(4) uint32_t pyramidalHeightMapId;
	};

	struct PushConstantDebug
	{
		alignas(16) glm::mat4 modelMatrix;
		alignas(16) glm::vec3 cameraPosition;
	};

	struct Particle
	{
		glm::vec2 position;
		glm::vec2 velocity;
		glm::vec4 color;
	};

	struct ComputeParticleUniformBufferObject
	{
		float deltaTime = 1.0f;
	};

	struct ComputeClusteredUniformBufferObject
	{
		alignas(16) glm::mat4 	view;
		alignas(16) glm::mat4 	proj;
		alignas(16) glm::mat4 	projInverse;
		alignas(8) glm::vec2 	screenDimensions;
		// alignas(4) uint32_t 	tileSizePx;
	};

	struct ClusterPlane
	{
		alignas(8) float	zNear;
		alignas(8) float	zFar;
	};

	struct ClusterGrid
	{
		alignas(16) glm::vec3	minPosition;
		alignas(16) glm::vec3	maxPosition;
	};

	struct LightObject
	{
		alignas(16) glm::vec3	position;
		alignas(16) glm::vec3	color;
		alignas(4) float 		intensity;
		alignas(4) float 		radius;
	};

	// LightGridIndex
	// uint	value;

	struct LightGrid
	{
		alignas(4) uint32_t offset;
		alignas(4) uint32_t size;
	};

	struct PyramidalHeightMap
	{
		alignas(4) uint32_t heightMapId;
		alignas(4) uint32_t width;
		alignas(4) uint32_t height;
		alignas(4) uint32_t mipLevels;
	};

	struct VLightVertex
	{
		alignas(16) glm::vec3	position;
		alignas(16) glm::vec3	vertexNormal;
	};
}
