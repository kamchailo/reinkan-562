#pragma once

#include <vulkan/vulkan.h>

namespace Reinkan::Graphics
{
	struct BufferWrap
	{
		VkBuffer buffer;
		VkDeviceMemory memory;

		void Destroy(VkDevice& device)
		{
			vkDestroyBuffer(device, buffer, nullptr);
			vkFreeMemory(device, memory, nullptr);
		}
	};
}
