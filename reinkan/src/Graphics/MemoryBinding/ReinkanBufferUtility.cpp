#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{

    BufferWrap ReinkanApp::CreateBufferWrap(VkDeviceSize size, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryProperties)
    {
        BufferWrap bufferWrap;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = bufferUsage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(appDevice, &bufferInfo, nullptr, &bufferWrap.buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(appDevice, bufferWrap.buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, memoryProperties);

        if (vkAllocateMemory(appDevice, &allocInfo, nullptr, &bufferWrap.memory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(appDevice, bufferWrap.buffer, bufferWrap.memory, 0);

        return bufferWrap;
    }

    BufferWrap ReinkanApp::CreateStagedBufferWrap(const VkDeviceSize& size, const void* data, VkBufferUsageFlags usage)
    {
        BufferWrap staging = CreateBufferWrap(size,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT // create buffer in host visible
                                              | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); // to auto clear cache

        void* dest;
        vkMapMemory(appDevice, staging.memory, 0, size, 0, &dest);
        // Copy data to hosst visible memory
        memcpy(dest, data, size);
        vkUnmapMemory(appDevice, staging.memory);

        BufferWrap localBufferWrap = CreateBufferWrap(size,
                                                      VK_BUFFER_USAGE_TRANSFER_DST_BIT 
                                                      | usage,
                                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); // create buffer at device local (not host visible)

        CopyBuffer(staging.buffer, localBufferWrap.buffer, size);

        staging.Destroy(appDevice);
        
        return localBufferWrap;
    }

    uint32_t ReinkanApp::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(appPhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
            {
                return i;
            }
        }
        throw std::runtime_error("failed to find suitable memory type!");
    }

    void ReinkanApp::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
    {
        auto tempCommandBuffer = BeginTempCommandBuffer();
        {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;
            vkCmdCopyBuffer(tempCommandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
        }
        EndTempCommandBuffer(tempCommandBuffer);
    }
}
