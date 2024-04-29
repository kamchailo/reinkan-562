#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CleanupPostProcessing()
    {
        for (auto& specialImageWrap : appSpecialFunctionImageWraps)
        {
            specialImageWrap.Destroy(appDevice);
        }

        appPostDescriptorWrap.Destroy(appDevice);

        vkDestroyPipeline(appDevice, appPostPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appPostPipelineLayout, nullptr);
        vkDestroyRenderPass(appDevice, appPostRenderPass, nullptr);
    }
}