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

            // only compute shader target does not need to recreate when resizing window
            appBlurShadowMapImageWraps[i].Destroy(appDevice);

            appShadowMapImageWraps[i].Destroy(appDevice);
            vkDestroyFramebuffer(appDevice, appShadowFrameBuffers[i], nullptr);
        }

        appShadowDepthImageWrap.Destroy(appDevice);

        appShadowDescriptorWrap.Destroy(appDevice);
        vkDestroyPipeline(appDevice, appShadowPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appShadowPipelineLayout, nullptr);

        vkDestroyRenderPass(appDevice, appShadowRenderPass, nullptr);

        appShadowBlurDescriptorWrap.Destroy(appDevice);
        vkDestroyPipeline(appDevice, appShadowBlurHorizontalPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appShadowBlurHorizontalPipelineLayout, nullptr);
        vkDestroyPipeline(appDevice, appShadowBlurVerticalPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appShadowBlurVerticalPipelineLayout, nullptr);
    }
}