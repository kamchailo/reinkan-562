#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
	void ReinkanApp::InitVulkan()
	{
		// Hardware Support Init
		
		CreateInstance();

		SetupDebugMessenger();

		CreateSurface();

		PickPhysicalDevice();

		CreateLogicalDevice();

		// Rendering Environment Init

		CreateSwapchain();

		CreateSwapchainImageViews();

		// Command or Drawing Resources Init

		CreateCommandPool();

		CreateCommandBuffer();

		CreateSyncObjects();

		// Render Pass
		CreateScanlineRenderPass();

		CreatePostRenderPass();

		CreateShadowRenderPass();

		CreateVLightingRenderPass();

		CreateDeferredLightingRenderPass();

		// MultiSampling
		CreateSwapchainColorResources();

		CreateSwapchainDepthResource();

		// Shadow
		CreateShadowResources(1024, 1024);

		CreateShadowBlurResources();

		CreateShadowCommandBuffer();

		CreateShadowSyncObjects();

		// From VolumicLighting
		CreateVLightResources(appShadowMapWidth, appShadowMapHeight);

		CreateSwapchainFrameBuffers(); // require renderpass and resources
		
		CreateScanlineFrameBuffers();

		CreateShadowFrameBuffers();

		CreateVLightFrameBuffers();

		CreateDeferredLightFrameBuffers();

		// Resources Binding will happen after this point
	}

	void ReinkanApp::BindResources()
	{
		BindModelData();

		BindMaterials();

		BindTextures();

		// Default Viewport UBO - Projection and View Matrix
		CreateScanlineUBO();

		// Global Light
		CreateComputeClusteredGlobalLights();

		// Shadow
		CreateShadowDescriptorSetWrap();

		CreateShadowPipeline(appShadowDescriptorWrap);

		// Shadow Blur
		CreateShadowBlurDescriptorSetWrap();

		CreateShadowBlurHorizontalPipeline(appShadowBlurDescriptorWrap);
		CreateShadowBlurVerticalPipeline(appShadowBlurDescriptorWrap);

		// Volumic Light Shaft
		CreateVLightDescriptorSetWrap();

		CreateVLightPipeline(appVLightDescriptorWrap);

		// Scanline
		CreateScanlineDescriptorWrap();

		CreateScanlinePipeline(appScanlineDescriptorWrap);

		// Deferred Lightinh
		CreateDeferredLightDescriptorSetWrap();

		CreateDeferredLightPipeline(appDeferredLightDescriptorWrap);

		// Post Processing
		CreatePostDescriptorSetWrap();

		CreatePostPipeline(appPostDescriptorWrap);

		// Debug
		CreateDebugBufferWraps();

		CreateDebugDescriptorSetWrap();

		CreateDebugPipeline(appDebugDescriptorWrap, 
							VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
							VK_POLYGON_MODE_LINE,
							1.0f);

		// Dummy Pipeline
		CreateDummyComputePipeline(100, 100);

		std::printf("\n=============================== END OF BIND RESOURCES ===============================\n\n");
		
	}

	bool ReinkanApp::ShouldClose()
	{
		return glfwWindowShouldClose(appWindow);
	}

	void ReinkanApp::ReinkanUpdate()
	{
		glfwPollEvents();

#ifdef GUI
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawGUI();
#endif

		DrawFrame();

	}
}

