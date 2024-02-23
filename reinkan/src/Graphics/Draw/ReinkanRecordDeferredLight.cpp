#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{

    void ReinkanApp::RecordDeferredLightPass(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = appDeferredLightRenderPass;
        renderPassBeginInfo.framebuffer = appDeferredLightFrameBuffers[imageIndex]; // change to scanline framebuffer
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = appSwapchainExtent;

        // Order match attachments
        std::array<VkClearValue, 1> clearValues{};
        clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
        //clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appDeferredLightPipeline);

            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                appDeferredLightPipelineLayout,
                0,
                1,
                &appDeferredLightDescriptorWrap.descriptorSets[appCurrentFrame],
                0,
                nullptr);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(appSwapchainExtent.width);
            viewport.height = static_cast<float>(appSwapchainExtent.height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

            VkRect2D scissor{};
            scissor.offset = { 0, 0 };
            scissor.extent = appSwapchainExtent;
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
            PushConstantDeferredLight pushConstant{};

            pushConstant.screenExtent = glm::vec2(appSwapchainExtent.width, appSwapchainExtent.height);
            pushConstant.cameraPosition = glm::vec4(appMainCamera->GetPosition(), 1.0);

            vkCmdPushConstants(commandBuffer,
                appDeferredLightPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantDeferredLight),
                &pushConstant
            );

            VkDeviceSize offsets[] = { 0 }; // make it cache friendly by bind all vertices together and use offset

            auto lightMesh = appLightMeshTypes.begin()->second;

            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &lightMesh.vertexBufferWrap.buffer, offsets);
            vkCmdBindIndexBuffer(commandBuffer, lightMesh.indexBufferWrap.buffer, 0, VK_INDEX_TYPE_UINT32);

            /*
            void vkCmdDrawIndexed(
                VkCommandBuffer                             commandBuffer,
                uint32_t                                    indexCount,
                uint32_t                                    instanceCount,
                uint32_t                                    firstIndex,
                int32_t                                     vertexOffset,
                uint32_t                                    firstInstance);
            */
            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(lightMesh.nbIndices), appLightObjects.size(), 0, 0, 0);

        }
        vkCmdEndRenderPass(commandBuffer);
    }
}