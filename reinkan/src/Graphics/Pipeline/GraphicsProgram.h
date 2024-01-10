#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include "Graphics/Descriptor/DescriptorWrap.h"

namespace Reinkan::Graphics
{
	class GraphicsProgram
	{
	public:

		GraphicsProgram(std::string vertexShaderPath,
						std::string fragmentShaderPath,
						DescriptorWrap descriptorWrap);

		VkPipelineLayout graphicPipelineLayout;

		VkPipeline graphicPipeline;

		DescriptorWrap graphicsDescriptorWrap;

	private:

		inline static uint32_t graphicProgramTotal;

		uint32_t graphicProgramId;

		void CreateGraphicPipelineLayout();

		void CreateGraphicPipeline(std::string vertexShaderPath, std::string fragmentShaderPath, DescriptorWrap descriptorWrap, VkPipelineLayout layout);
	};
}