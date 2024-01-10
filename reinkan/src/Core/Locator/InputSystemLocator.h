#pragma once

#include "Input/InputSystem.h"

namespace Reinkan::Core
{
	class InputSystemLocator
	{
	public:
		static Input::InputSystem* GetInputSystem()
		{
			return inputSystemInstance;
		}

		static void Provide(Input::InputSystem* inputSystem)
		{
			inputSystemInstance = inputSystem;
		}

	private:
		inline static Input::InputSystem* inputSystemInstance{ nullptr };
	};
}