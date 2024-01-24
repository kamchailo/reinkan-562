#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
	void ReinkanApp::DestroyDeferredLightResources()
	{

		appDeferredLightDescriptorWrap.Destroy(appDevice);

		vkDestroyPipeline(appDevice, appDeferredLightPipeline, nullptr);
		vkDestroyPipelineLayout(appDevice, appDeferredLightPipelineLayout, nullptr);

		vkDestroyRenderPass(appDevice, appDeferredLightRenderPass, nullptr);
	}
}