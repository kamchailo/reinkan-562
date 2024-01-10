#pragma once

#include <vector>
#include <string>
#include <glm/mat4x4.hpp>

#include "../shaders/SharedStruct.h"

namespace Reinkan::Graphics
{
	struct ModelData
	{
		std::vector<Vertex> vertices;

		std::vector<uint32_t> indices;
		
		uint32_t materialIndex;

		std::string name;
	};

	// For future ModelToBeLoaded
	// if there are more instance property then modelTransform
	struct ModelDataLoading
	{
		std::shared_ptr<ModelData> modelDataPtr;
		
		glm::mat4 modelTransform;
	};
}