#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::RecordVLightPass(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = appVLightRenderPass;
        renderPassBeginInfo.framebuffer = appVLightFrameBuffers[imageIndex]; // change to scanline framebuffer
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
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appVLightPipeline);

            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                appVLightPipelineLayout,
                0,
                1,
                &appVLightDescriptorWrap.descriptorSets[appCurrentFrame],
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
            PushConstantVLight pushConstant{};

            pushConstant.lightPosition = glm::vec4(appGlobalLightPosition, 1.0);
            pushConstant.lightConstant = glm::vec4(1, 0.9, 0.5, 1);
            pushConstant.cameraPosition = glm::vec4(appMainCamera->GetPosition(), 1.0);
            pushConstant.shadowMapExtent = glm::vec2(appShadowMapWidth, appShadowMapHeight);
            pushConstant.lightDistanceScale = appVLightDistanceScale;
            pushConstant.lightRadius = appVLightRadius;
            pushConstant.debugFloat = appDebugFloat;

            vkCmdPushConstants(commandBuffer,
                appVLightPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantVLight),
                &pushConstant
            );

            VkDeviceSize offsets[] = { 0 }; // make it cache friendly by bind all vertices together and use offset
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, &appVLightVertexBufferWrap.buffer, offsets);
            vkCmdBindIndexBuffer(commandBuffer, appVLightIndexBufferWrap.buffer, 0, VK_INDEX_TYPE_UINT32);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(appVLightIndices.size()), 1, 0, 0, 0);

        }
        vkCmdEndRenderPass(commandBuffer);
    }
}