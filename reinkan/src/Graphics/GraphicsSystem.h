#pragma once

#include "Graphics/ReinkanVulkan.h"

namespace Reinkan::Graphics
{
	class GraphicsSystem
	{
	public:
		void Init(uint32_t width, uint32_t height);

		// going to be only input channel to Graphics System
		void LoadResources();

		void Update();

		void Destroy();

		ReinkanApp* GetVulkanApp() const;

	private:
		std::shared_ptr<ReinkanApp> vulkanApp;
	};
}