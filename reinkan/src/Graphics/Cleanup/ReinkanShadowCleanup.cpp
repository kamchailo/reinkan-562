#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::DestroyShadowResources()
    {
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appShadowUBO[i].Destroy(appDevice);
            appShadowBlurUBO[i].Destroy(appDevice);

            vkDestroySemaphore(appDevice, appPreComputeFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(appDevice, appComputeShadowBlurFinishedSemaphores[i], nullptr);
            vkDestroyFence(appDevice, appRenderShadowFences[i], nullptr);
            vkDestroyFence(appDevice, appComputeShadowBlurFences[i], nullptr);

            // Move to clean up swapchain for frame resizing
            //appShadowMapImageWraps[i].Destroy(appDevice);
            //vkDestroyFramebuffer(appDevice, appShadowFrameBuffers[i], nullptr);
        }

        appShadowDescriptorWrap.Destroy(appDevice);
        vkDestroyPipeline(appDevice, appShadowPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appShadowPipelineLayout, nullptr);

        vkDestroyRenderPass(appDevice, appShadowRenderPass, nullptr);

        appShadowBlurDescriptorWrap.Destroy(appDevice);
        vkDestroyPipeline(appDevice, appShadowBlurPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appShadowBlurPipelineLayout, nullptr);
    }
}