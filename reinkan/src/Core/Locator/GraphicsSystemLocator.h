#pragma once

#include "Graphics/GraphicsSystem.h"

namespace Reinkan::Core
{
	class GraphicsSystemLocator
	{
	public:
		static Graphics::GraphicsSystem* GetGraphicsSystem()
		{
			return graphicsSystemInstance;
		}

		static void Provide(Graphics::GraphicsSystem* graphicsSystem)
		{
			graphicsSystemInstance = graphicsSystem;
		}

	private:
		inline static Graphics::GraphicsSystem* graphicsSystemInstance{ nullptr };
	};
}