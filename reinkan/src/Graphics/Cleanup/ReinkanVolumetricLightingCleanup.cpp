#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
	void ReinkanApp::DestroyVLightResources()
	{
		appVLightVertexBufferWrap.Destroy(appDevice);

		appVLightIndexBufferWrap.Destroy(appDevice);

		//for (int i = 0; i < appVLightingRenderTargetImageWraps.size(); ++i)
		//{
			//appVLightingRenderTargetImageWraps[i].Destroy(appDevice);
		//}

		appVLightDescriptorWrap.Destroy(appDevice);
		vkDestroyPipelineLayout(appDevice, appVLightPipelineLayout, nullptr);
		vkDestroyPipeline(appDevice, appVLightPipeline, nullptr);

		//for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		//{
			//vkDestroyFramebuffer(appDevice, appVLightFrameBuffers[i], nullptr);
		//}

		vkDestroyRenderPass(appDevice, appVLightRenderPass, nullptr);
	}
}