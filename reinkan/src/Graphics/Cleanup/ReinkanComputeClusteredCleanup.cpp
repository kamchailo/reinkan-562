#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::DestroyComputeClusteredResources()
    {
        //Resources
        appClusteredGridDescriptorWrap.Destroy(appDevice);
        appClusteredCullLightDescriptorWrap.Destroy(appDevice);

        appClusteredPlanes.Destroy(appDevice);
        appClusteredGlobalLights.Destroy(appDevice);

        appClusteredGlobalLights_DEBUG.Destroy(appDevice);

        appClusteredGrids.Destroy(appDevice);
        
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            appClusteredUBO[i].Destroy(appDevice);


            if (appLightObjects.size() == 0) { break; }
            
            appClusteredLightIndexMap[i].Destroy(appDevice);

            appClusteredLightGrid[i].Destroy(appDevice);

            appClusteredGlobalIndexCount[i].Destroy(appDevice);
        }

        // Pipeline
        vkDestroyPipeline(appDevice, appClusteredGridPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appClusteredGridPipelineLayout, nullptr);

        vkDestroyPipeline(appDevice, appClusteredCullLightPipeline, nullptr);
        vkDestroyPipelineLayout(appDevice, appClusteredCullLightPipelineLayout, nullptr);

        // Sync
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(appDevice, appComputeClusteredFinishedSemaphores[i], nullptr);
            vkDestroyFence(appDevice, appComputeClusteredInFlightFences[i], nullptr);
        }
    }
}