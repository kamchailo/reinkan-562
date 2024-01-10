#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::RecordPostProcessing(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {

        VkRenderPassBeginInfo renderPassBeginInfo{};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = appPostRenderPass;
        renderPassBeginInfo.framebuffer = appSwapchainFramebuffers[imageIndex]; // Output to Swapchain
        renderPassBeginInfo.renderArea.offset = { 0, 0 };
        renderPassBeginInfo.renderArea.extent = appSwapchainExtent;

        // Order match attachments
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { {0.05f, 0.07f, 0.05f, 1.0f} };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassBeginInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, appPostPipeline);

            vkCmdBindDescriptorSets(commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                appPostPipelineLayout,
                0,
                1,
                &appPostDescriptorWrap.descriptorSets[appCurrentFrame],
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

            PushConstantPost pushConstant;
            pushConstant.screenExtent = glm::vec2(appSwapchainExtent.width, appSwapchainExtent.height);
            pushConstant.debugFlag = appDebugFlag;
            pushConstant.debugFloat = appDebugFloat;
            pushConstant.debugFloat2 = appDebugFloat2;
            pushConstant.debugFloat3 = appDebugFloat3;

            vkCmdPushConstants(commandBuffer,
                appPostPipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantPost),
                &pushConstant
            );

            // Draw Imaginary Vertices
            /// Access vertex instances with gl_VertexIndex
            vkCmdDraw(commandBuffer, 3, 1, 0, 0);


#ifdef GUI
            ImGui::Render();  // Rendering UI
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
#endif

        }
        vkCmdEndRenderPass(commandBuffer);
    }
}