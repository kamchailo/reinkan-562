#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::RecordShadowPass(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = appShadowRenderPass;
        renderPassBeginInfo.framebuffer = appShadowFrameBuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent.width = appShadowMapWidth;
        renderPassBeginInfo.renderArea.extent.height = appShadowMapHeight;

        // Order match attachments
        std::array<VkClearValue, 2> clearValues{};
        //clearValues[0].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
        clearValues[0].color = { {appGlobalLightPosition.x, appGlobalLightPosition.y, appGlobalLightPosition.z, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appShadowPipeline);

            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                appShadowPipelineLayout,
                0,
                1,
                &appShadowDescriptorWrap.descriptorSets[appCurrentFrame],
                0,
                nullptr);

            
            for (auto object : appObjects)
            {
                // Push Constant
                // Information pushed at each draw call
                PushConstantShadow pushConstant{};
                //pushConstant.materialId = object.materialId;
                //pushConstant.objectId = object.objectId;
                pushConstant.modelMatrix = object.transform;
                pushConstant.screenExtent = glm::vec2(appShadowMapWidth, appShadowMapHeight);
                //pushConstant.normalMatrix = glm::transpose(glm::inverse(glm::mat3(object.transform)));
                //pushConstant.debugFlag = appDebugFlag;
                //pushConstant.debugFloat = appDebugFloat;
                //pushConstant.debugFloat2 = appDebugFloat2;
                //pushConstant.debugInt = appDebugInt;

                vkCmdPushConstants(commandBuffer,
                    appShadowPipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(PushConstantShadow),
                    &pushConstant
                );

                VkDeviceSize offsets[] = { 0 }; // make it cache friendly by bind all vertices together and use offset
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBufferWrap.buffer, offsets);
                vkCmdBindIndexBuffer(commandBuffer, object.indexBufferWrap.buffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(object.nbIndices), 1, 0, 0, 0);

            }
        }
        vkCmdEndRenderPass(commandBuffer);
    }
}