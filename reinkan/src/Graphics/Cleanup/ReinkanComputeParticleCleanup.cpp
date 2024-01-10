#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
    void ReinkanApp::DestroyComputeParticleResources()
    {
		// Resources
		appComputeParticleDescriptorWrap.Destroy(appDevice);
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			appComputeParticleUBO[i].Destroy(appDevice);
			appComputeParticleStorageBufferWraps[i].Destroy(appDevice);
		}
		// Pipeline
		vkDestroyPipeline(appDevice, appComputeParticlePipeline, nullptr);
		vkDestroyPipelineLayout(appDevice, appComputeParticlePipelineLayout, nullptr);
		// Sync
		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(appDevice, appComputeParticleFinishedSemaphores[i], nullptr);
			vkDestroyFence(appDevice, appComputeParticleInFlightFences[i], nullptr);
		}
    }
}