#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
	void ReinkanApp::Cleanup()
	{
		std::printf("\n=============================== START OF CLEAN UP ===============================\n\n");

		vkDeviceWaitIdle(appDevice);

		CleanupSwapchain();

		// Debug
		DestroyDebugResources();

		// Global Light
		appClusteredGlobalLights.Destroy(appDevice);
		appClusteredGlobalLights_DEBUG.Destroy(appDevice);

		// ParallaxOcclusion
		DestroyParallaxOcclusionResources();

		// Shadow
		DestroyShadowResources();

		// VolumetricLighting
		DestroyVLightResources();

		// GlobalLight
		DestroyGlobalLightResources();

		// DeferredLighting
		DestroyDeferredLightResources();

		// Ambient Occlusion
		DestroyAOResources();

		// Post Processing
		CleanupPostProcessing();

		{ // Scanline 
		// Resources
			for (auto textureImageWrap : appTextureImageWraps)
				textureImageWrap.Destroy(appDevice);

			appMaterialBufferWrap.Destroy(appDevice);
			appScanlineDescriptorWrap.Destroy(appDevice);
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
			{
				appScanlineUBO[i].Destroy(appDevice);
			}

			for (auto object : appObjects)
			{
				object.vertexBufferWrap.Destroy(appDevice);
				object.indexBufferWrap.Destroy(appDevice);
			}

		// Light Object
			for (auto lightMesh : appLightMeshTypes)
			{
				lightMesh.second.vertexBufferWrap.Destroy(appDevice);
				lightMesh.second.indexBufferWrap.Destroy(appDevice);
			}

		// Pipeline & RenderPass
			vkDestroyPipeline(appDevice, appScanlinePipeline, nullptr);
			vkDestroyPipelineLayout(appDevice, appScanlinePipelineLayout, nullptr);
			vkDestroyRenderPass(appDevice, appScanlineRenderPass, nullptr);


		// Sync
			for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
			{
				vkDestroySemaphore(appDevice, renderFinishedSemaphores[i], nullptr);
				vkDestroySemaphore(appDevice, imageAvailableSemaphores[i], nullptr);
				vkDestroyFence(appDevice, inFlightFences[i], nullptr);
			}
		} // End Scanline Resources

		// Compute Command Pool
		vkDestroyCommandPool(appDevice, appComputeCommandPool, nullptr);
		vkDestroyCommandPool(appDevice, appCommandPool, nullptr);

		#ifdef GUI
		DestroyGUI();
		#endif

		// Destroy Foundation
		vkDestroyDevice(appDevice, nullptr);

		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(appInstance, appDebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(appInstance, appSurface, nullptr);
		vkDestroyInstance(appInstance, nullptr);

		glfwDestroyWindow(appWindow);

		glfwTerminate();
	}
}