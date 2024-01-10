#pragma once

#include <glm/glm.hpp>

#include "Graphics/Structure/BufferWrap.h"

namespace Reinkan::Graphics
{
	struct ObjectData
	{
		uint32_t        nbIndices{0};

        uint32_t        nbVertices{0};

        BufferWrap      vertexBufferWrap;
   
        BufferWrap      indexBufferWrap;
    
        std::string     name;

        uint32_t        objectId;

        uint32_t        materialId;

        glm::mat4       transform;
	};
}