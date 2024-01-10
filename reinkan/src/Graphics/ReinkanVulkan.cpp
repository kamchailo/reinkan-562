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

		// MultiSampling
		CreateSwapchainColorResources();

		CreateSwapchainDepthResource();

		// Shadow
		CreateShadowResources(1024, 1024);

		// From VolumicLighting
		CreateVLightResources(appShadowMapWidth, appShadowMapHeight);

		CreateSwapchainFrameBuffers(); // require renderpass and resources
		
		CreateScanlineFrameBuffers();

		CreateShadowFrameBuffers();

		CreateVLightFrameBuffers();

		// Resources Binding will happen after this point
	}

	void ReinkanApp::BindResources()
	{

		{
			BindModelData();

			BindMaterials();

			BindTextures();

			// Default Viewport UBO - Projection and View Matrix
			CreateScanlineUBO();

			// Clustered
			CreateComputeClusteredBufferWraps(16, 9, 32, 0.1, 1000.0);

			CreateComputeClusteredDescriptorSetWrap();

			CreateClusteredGridPipeline(appClusteredGridDescriptorWrap);

			CreateClusteredCullLightPipeline(appClusteredCullLightDescriptorWrap);

			CreateComputeClusteredSyncObjects();

			// Shadow
			CreateShadowDescriptorSetWrap();

			CreateShadowPipeline(appShadowDescriptorWrap);

			// Volumic Light Shaft
			CreateVLightDescriptorSetWrap();

			CreateVLightPipeline(appVLightDescriptorWrap);

			// Scanline
			CreateScanlineDescriptorWrap();

			CreateScanlinePipeline(appScanlineDescriptorWrap);

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

			std::printf("\n=============================== END OF BIND RESOURCES ===============================\n\n");
		}
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

