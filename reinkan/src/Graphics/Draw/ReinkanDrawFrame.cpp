#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include "Graphics/ParticleSystem/ParticleSystemConstant.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::DrawFrame()
    {
        // Skip draw if fence is not ready
        /*
        if (vkGetFenceStatus(appDevice, inFlightFences[appCurrentFrame]) == VK_NOT_READY)
        {
            std::printf("Skip Frame %d: Ongoing Rendering\n", appCurrentFrame);
            #ifdef GUI
            ImGui::EndFrame();
            #endif
            return;
        }
        */

        ////////////////////////////////////////
        //          Acquiring Image 
        ////////////////////////////////////////

        // can only pass if inFlightFences is [SIGNAL]
        vkWaitForFences(appDevice, 1, &inFlightFences[appCurrentFrame], VK_TRUE, UINT64_MAX);
        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(appDevice,
            appSwapchain,
            UINT64_MAX,
            imageAvailableSemaphores[appCurrentFrame],
            VK_NULL_HANDLE,
            &imageIndex);
        // After finish on GPU
        // > > > > > > > [SIGNAL] imageAvailableSemaphores[appCurrentFrame]

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            appFramebufferResized = false;

            RecreateSwapchain();
#ifdef GUI
            ImGui::EndFrame();
#endif  

            return;
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        ////////////////////////////////////////
        //          Pre Compute -- Shadow Pass
        ////////////////////////////////////////
        vkWaitForFences(appDevice, 1, &appRenderShadowFences[appCurrentFrame], VK_TRUE, UINT64_MAX);

        UpdateShadowUBO(appCurrentFrame); // Update shadow first for appShadowProjectionViewMatrix

        UpdateAOBlurUBO(appCurrentFrame);
        
        vkResetFences(appDevice, 1, &appRenderShadowFences[appCurrentFrame]);

        vkResetCommandBuffer(appPreComputeCommandBuffer[appCurrentFrame], 0);
        VkCommandBufferBeginInfo beginShadowInfo{};
        beginShadowInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(appPreComputeCommandBuffer[appCurrentFrame], &beginShadowInfo) != VK_SUCCESS)
        { throw std::runtime_error("failed to begin recording command buffer!"); }
        // --------------------
        // Pre Compute . . .
        // --------------------
        {
            RecordShadowPass(appPreComputeCommandBuffer[appCurrentFrame], appCurrentFrame);
            
            RecordScanline(appPreComputeCommandBuffer[appCurrentFrame], appCurrentFrame);

            RecordAOPass(appPreComputeCommandBuffer[appCurrentFrame], appCurrentFrame);

        }
        if (vkEndCommandBuffer(appPreComputeCommandBuffer[appCurrentFrame]) != VK_SUCCESS)
        { throw std::runtime_error("failed to record command buffer!"); }

        //VkSemaphore computeWaitSemaphores[] = { };
        VkSemaphore preComputeSignalSemaphores[] = { appPreComputeFinishedSemaphores[appCurrentFrame] };
        //VkPipelineStageFlags preComputeWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitPreComputeInfo{};
        submitPreComputeInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitPreComputeInfo.commandBufferCount = 1;
        submitPreComputeInfo.pCommandBuffers = &appPreComputeCommandBuffer[appCurrentFrame];
        submitPreComputeInfo.waitSemaphoreCount = 0;
        submitPreComputeInfo.pWaitSemaphores = {};
        submitPreComputeInfo.signalSemaphoreCount = 1;
        submitPreComputeInfo.pSignalSemaphores = preComputeSignalSemaphores;
        if (vkQueueSubmit(appGraphicsQueue, 1, &submitPreComputeInfo, appRenderShadowFences[appCurrentFrame]) != VK_SUCCESS)
        { throw std::runtime_error("failed to submit compute command buffer!"); };
        
        ////////////////////////////////////////
        //          Compute Dispatch
        ////////////////////////////////////////
        vkWaitForFences(appDevice, 1, &appComputeShadowBlurFences[appCurrentFrame], VK_TRUE, UINT64_MAX);

        // Upload CPU resources
        UpdateShadowBlurUBO(appCurrentFrame);
        vkResetFences(appDevice, 1, &appComputeShadowBlurFences[appCurrentFrame]);

        vkResetCommandBuffer(appComputeCommandBuffers[appCurrentFrame], 0);

        VkCommandBufferBeginInfo beginComputeInfo{};
        beginComputeInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(appComputeCommandBuffers[appCurrentFrame], &beginComputeInfo) != VK_SUCCESS)
        { throw std::runtime_error("failed to begin recording command buffer!"); }
        
        // --------------------
        // Compute . . .
        // --------------------
        {
            // Horizontal Shadow Blur Compute Dispatch
            RecordComputeCommandBuffer(appComputeCommandBuffers[appCurrentFrame],
                                       appShadowBlurHorizontalPipeline,
                                       appShadowBlurHorizontalPipelineLayout,
                                       appShadowBlurDescriptorWrap,
                                       appShadowMapWidth / 128, appShadowMapHeight, 1,
                                       true);
            
            CmdCopyImage(appComputeCommandBuffers[appCurrentFrame], 
                         appBlurShadowMapImageWraps[appCurrentFrame], 
                         appShadowMapImageWraps[appCurrentFrame],
                         appShadowMapWidth,
                         appShadowMapHeight); 
            
            // Vertical Shadow Blur Compute Dispatch
            RecordComputeCommandBuffer(appComputeCommandBuffers[appCurrentFrame],
                                       appShadowBlurVerticalPipeline,
                                       appShadowBlurVerticalPipelineLayout,
                                       appShadowBlurDescriptorWrap,
                                       appShadowMapWidth, appShadowMapHeight / 128, 1,
                                       false);
            
            // Horizontal AO Blur Compute Dispatch
            RecordComputeCommandBuffer(appComputeCommandBuffers[appCurrentFrame],
                                       appAOBlurHorizontalPipeline,
                                       appAOBlurHorizontalPipelineLayout,
                                       appAOBlurDescriptorWrap,
                                       appSwapchainExtent.width / 128, appSwapchainExtent.height, 1,
                                       true);
            
            CmdCopyImage(appComputeCommandBuffers[appCurrentFrame],
                         appBlurAOMapImageWraps[appCurrentFrame],
                         appAORenderTargetImageWraps[appCurrentFrame],
                         appSwapchainExtent.width,
                         appSwapchainExtent.height);
            

            // Vertical AO Blur Compute Dispatch
            RecordComputeCommandBuffer(appComputeCommandBuffers[appCurrentFrame],
                                       appAOBlurVerticalPipeline,
                                       appAOBlurVerticalPipelineLayout,
                                       appAOBlurDescriptorWrap,
                                       appSwapchainExtent.width, appSwapchainExtent.height / 128, 1,
                                       false);
            
            
            // IBL Compute Dispatch
        }
        
        if (vkEndCommandBuffer(appComputeCommandBuffers[appCurrentFrame]) != VK_SUCCESS)
        { throw std::runtime_error("failed to record command buffer!"); }
        

        VkSemaphore computeWaitSemaphores[] = { appPreComputeFinishedSemaphores[appCurrentFrame] };
        VkSemaphore computeSignalSemaphores[] = { appComputeShadowBlurFinishedSemaphores[appCurrentFrame] };
        VkPipelineStageFlags computeWaitStages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };

        VkSubmitInfo submitComputeInfo{};
        submitComputeInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitComputeInfo.pNext;
        submitComputeInfo.waitSemaphoreCount = 1;
        submitComputeInfo.pWaitSemaphores = computeWaitSemaphores;
        submitComputeInfo.pWaitDstStageMask = computeWaitStages;
        submitComputeInfo.commandBufferCount = 1;
        submitComputeInfo.pCommandBuffers = &appComputeCommandBuffers[appCurrentFrame];
        submitComputeInfo.signalSemaphoreCount = 1;
        submitComputeInfo.pSignalSemaphores = computeSignalSemaphores;
        if (vkQueueSubmit(appComputeQueue, 1, &submitComputeInfo, appComputeShadowBlurFences[appCurrentFrame]) != VK_SUCCESS)
        { throw std::runtime_error("failed to submit compute command buffer!"); };
        

        ////////////////////////////////////////
        //          Graphics Draw
        ////////////////////////////////////////

        // --------------------
        // Update Value per frame
        // --------------------
        UpdateScanlineUBO(appCurrentFrame);
        UpdateShadowUBO(appCurrentFrame); // Update shadow first for appShadowProjectionViewMatrix

        // Only reset the fence if we are submitting work
        // [WAIT] inFlightFences[appCurrentFrame] or [UNSIGNAL]
        vkResetFences(appDevice, 1, &inFlightFences[appCurrentFrame]);

        vkResetCommandBuffer(appCommandBuffers[appCurrentFrame], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(appCommandBuffers[appCurrentFrame], &beginInfo) != VK_SUCCESS)
        { throw std::runtime_error("failed to begin recording command buffer!"); }
        {
            // RecordShadowPass move to pre compute
            //RecordShadowPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);


            RecordGlobalLightPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordDeferredLightPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordVLightPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordPostProcessing(appCommandBuffers[appCurrentFrame], imageIndex);

        }
        if (vkEndCommandBuffer(appCommandBuffers[appCurrentFrame]) != VK_SUCCESS)
        { throw std::runtime_error("failed to record command buffer!"); }

        // Wait for Compute Shader
        //VkSemaphore waitSemaphores[] = { appComputeClusteredFinishedSemaphores[appCurrentFrame], imageAvailableSemaphores[appCurrentFrame] };
        //VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[appCurrentFrame] };
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[appCurrentFrame]
                                        ,appComputeShadowBlurFinishedSemaphores[appCurrentFrame]
                                       };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
                                            ,VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                                        };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //submitInfo.waitSemaphoreCount = 1;
        // With Compute
        submitInfo.waitSemaphoreCount = 2;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &appCommandBuffers[appCurrentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // [WAIT] imageAvailableSemaphores[appCurrentFrame]
        if (vkQueueSubmit(appGraphicsQueue, 1, &submitInfo, inFlightFences[appCurrentFrame]) != VK_SUCCESS)
        { throw std::runtime_error("failed to submit draw command buffer!"); }
        // After finish on GPU
        // > > > > > > > [SIGNAL] renderFinishedSemaphores[appCurrentFrame]

        VkSwapchainKHR swapchains[] = { appSwapchain };
        VkSemaphore waitPresentSemaphores[] = { renderFinishedSemaphores[appCurrentFrame] };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = waitPresentSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        // [WAIT] renderFinishedSemaphores[appCurrentFrame]
        vkQueuePresentKHR(appPresentQueue, &presentInfo);
        // > > > > > > > [SIGNAL] inFlightFences[appCurrentFrame]

        appCurrentFrame = (appCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}