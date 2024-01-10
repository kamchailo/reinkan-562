#include "pch.h"
#include "InputSystem.h"

#include <glm/vec2.hpp>

namespace Reinkan::Input
{
	void InputSystem::Init()
	{
	}

	void InputSystem::Update()
	{
		// Dampen Mouse Delta over time
		mouseDelta.x -= mouseDelta.x / 2.0f;
		mouseDelta.y -= mouseDelta.y / 2.0f;
	}

	void InputSystem::Destroy()
	{
	}

	bool InputSystem::IsKeyPressed(KeyCode keyCode)
	{
		return (KeyStateArray[keyCode] == KeyState::Pressed);
	}

	bool InputSystem::IsKeyRelease(KeyCode keyCode)
	{
		return (KeyStateArray[keyCode] == KeyState::Released);
	}

	bool InputSystem::IsKeyHeld(KeyCode keyCode)
	{
		return (KeyStateArray[keyCode] == KeyState::Hold || KeyStateArray[keyCode] == KeyState::Pressed);
	}

	bool InputSystem::IsMouseButtonPressed(MousButtonCode mouseButton)
	{
		return (mouseButtonStateArray[mouseButton] == KeyState::Pressed);
	}

	bool InputSystem::IsMouseButtonRelease(MousButtonCode mouseButton)
	{
		return (mouseButtonStateArray[mouseButton] == KeyState::Released);
	}

	bool InputSystem::IsMouseButtonHeld(MousButtonCode mouseButton)
	{
		return (mouseButtonStateArray[mouseButton] == KeyState::Hold || mouseButtonStateArray[mouseButton] == KeyState::Pressed);
	}

	void InputSystem::KeyCallBack(KeyCode keyCode, KeyState action)
	{
		// Lookup key in case we use engine custom keyCode
		// KeyCode keyCode = GetKeyCode(key);
		if (action == Released)
		{
			KeyStateArray[keyCode] = KeyState::Released;
			return;
		}

		if (KeyStateArray[keyCode] == KeyState::Pressed || KeyStateArray[keyCode] == KeyState::Hold)
		{
			KeyStateArray[keyCode] = KeyState::Hold;
		}
		else
		{
			KeyStateArray[keyCode] = KeyState::Pressed;
		}
	}

	void InputSystem::MouseButtonCallBack(MousButtonCode mouseButtonCode, KeyState action)
	{
		if (action == Released)
		{
			mouseButtonStateArray[mouseButtonCode] = KeyState::Released;
			return;
		}

		if (mouseButtonStateArray[mouseButtonCode] == KeyState::Pressed || mouseButtonStateArray[mouseButtonCode] == KeyState::Hold)
		{
			mouseButtonStateArray[mouseButtonCode] = KeyState::Hold;
		}
		else
		{
			mouseButtonStateArray[mouseButtonCode] = KeyState::Pressed;
		}
	}

	void InputSystem::SetMousePosition(const double x, const double y)
	{
		mousePosition = glm::vec2(x, y);
	}

	glm::vec2 InputSystem::GetMousePosition() const
	{
		return mousePosition;
	}

	glm::vec2 InputSystem::GetMouseDelta() const
	{
		return mouseDelta;
	}

	void InputSystem::MouseMove(const double x, const double y)
	{
		mouseDelta.x = x - mousePosition.x;
		mouseDelta.y = y - mousePosition.y;

		mousePosition.x = x;
		mousePosition.y = y;
	}
}

