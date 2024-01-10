#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::CleanupPostProcessing()
    {


        appPostDescriptorWrap.Destroy(appDevice);


        vkDestroyPipeline(appDevice, appPostPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appPostPipelineLayout, nullptr);
        vkDestroyRenderPass(appDevice, appPostRenderPass, nullptr);
    }
}