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

        if (!appIsClusteredGridReady)
        {
            //UpdateClusteredGrids();
            appIsClusteredGridReady = true;
        }


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
            appIsClusteredGridReady = false;

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
        //          Compute Dispatch
        ////////////////////////////////////////

        // --------------------
        // Compute . . .
        // --------------------

        ////////////////////////////////////////
        //          Graphics Draw
        ////////////////////////////////////////

        // --------------------
        // Update Value per frame
        // --------------------
        UpdateShadowUBO(appCurrentFrame); // Update shadow first for appShadowProjectionViewMatrix

        UpdateScanlineUBO(appCurrentFrame);

        // Only reset the fence if we are submitting work
        // [WAIT] inFlightFences[appCurrentFrame] or [UNSIGNAL]
        vkResetFences(appDevice, 1, &inFlightFences[appCurrentFrame]);

        vkResetCommandBuffer(appCommandBuffers[appCurrentFrame], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(appCommandBuffers[appCurrentFrame], &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        {

            RecordShadowPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordScanline(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordDeferredLightPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordVLightPass(appCommandBuffers[appCurrentFrame], appCurrentFrame);

            RecordPostProcessing(appCommandBuffers[appCurrentFrame], imageIndex);

        }
        if (vkEndCommandBuffer(appCommandBuffers[appCurrentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }

        // Wait for Compute Shader
        //VkSemaphore waitSemaphores[] = { appComputeClusteredFinishedSemaphores[appCurrentFrame], imageAvailableSemaphores[appCurrentFrame] };
        //VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[appCurrentFrame] };
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[appCurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //submitInfo.waitSemaphoreCount = 1;
        // With Compute
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &appCommandBuffers[appCurrentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        // [WAIT] imageAvailableSemaphores[appCurrentFrame]
        if (vkQueueSubmit(appGraphicsQueue, 1, &submitInfo, inFlightFences[appCurrentFrame]) != VK_SUCCESS)
        // After finish on GPU
        // > > > > > > > [SIGNAL] renderFinishedSemaphores[appCurrentFrame]
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        // Do Post Processing here

        VkSwapchainKHR swapchains[] = { appSwapchain };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex;

        // [WAIT] renderFinishedSemaphores[appCurrentFrame]
        vkQueuePresentKHR(appPresentQueue, &presentInfo);
        // > > > > > > > [SIGNAL] inFlightFences[appCurrentFrame]

        appCurrentFrame = (appCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}