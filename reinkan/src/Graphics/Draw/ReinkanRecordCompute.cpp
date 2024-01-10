#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include "Graphics/ParticleSystem/ParticleSystemConstant.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::RecordComputeCommandBuffer(VkCommandBuffer commandBuffer,
                                                VkPipeline pipeline,
                                                VkPipelineLayout pipelineLayout,
                                                DescriptorWrap descriptorWrap,
                                                uint32_t dispatchCountX,
                                                uint32_t dispatchCountY,
                                                uint32_t dispatchCountZ,
                                                bool isMemBarrier)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording compute command buffer!");
        }
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_COMPUTE,
                pipelineLayout,
                0,
                1,
                &descriptorWrap.descriptorSets[appCurrentFrame],
                0,
                nullptr);

            vkCmdDispatch(commandBuffer, dispatchCountX, dispatchCountY, dispatchCountZ);
        }

        if (isMemBarrier)
        {
            VkMemoryBarrier  memoryBarrier = {};
            memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            memoryBarrier.pNext = nullptr;
            memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
            memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_DEPENDENCY_DEVICE_GROUP_BIT,
                1,
                &memoryBarrier,
                0,
                nullptr,
                0,
                nullptr);
        }

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record compute command buffer!");
        }
    }
}