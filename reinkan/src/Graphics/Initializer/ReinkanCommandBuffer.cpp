#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CreateCommandPool() 
    {
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(appPhysicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

        if (vkCreateCommandPool(appDevice, &poolInfo, nullptr, &appCommandPool) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create command pool!");
        }

        // Create Optional Command Pool for Compute Shader
        if (queueFamilyIndices.computeOnlyFamily.has_value())
        {
            VkCommandPoolCreateInfo computePoolInfo{};
            computePoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            computePoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            computePoolInfo.queueFamilyIndex = queueFamilyIndices.computeOnlyFamily.value();

            if (vkCreateCommandPool(appDevice, &computePoolInfo, nullptr, &appComputeCommandPool) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create command pool!");
            }
        }
    }

    void ReinkanApp::CreateCommandBuffer() 
    {
        appCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = appCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)appCommandBuffers.size();;

        if (vkAllocateCommandBuffers(appDevice, &allocInfo, appCommandBuffers.data()) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    VkCommandBuffer ReinkanApp::BeginTempCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = appCommandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer cmdBuffer;
        vkAllocateCommandBuffers(appDevice, &allocateInfo, &cmdBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(cmdBuffer, &beginInfo);

        return cmdBuffer;
    }

    void ReinkanApp::EndTempCommandBuffer(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // appGraphicsQueue should be change to dedicate queue for Initialzation
        vkQueueSubmit(appGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(appGraphicsQueue);

        vkFreeCommandBuffers(appDevice, appCommandPool, 1, &commandBuffer);
    }


}
