#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::DestroyDebugResources()
    {
        appDebugDescriptorWrap.Destroy(appDevice);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appDebugUBO[i].Destroy(appDevice);

        }

        vkDestroyPipeline(appDevice, appDebugPipeline, nullptr);

        vkDestroyPipelineLayout(appDevice, appDebugPipelineLayout, nullptr);
    }
}