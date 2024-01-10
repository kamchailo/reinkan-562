#pragma once

#include <unordered_map>
#include <glm/vec2.hpp>

#include "InputConstant.h"

namespace Reinkan::Input
{

	typedef uint32_t KeyCode;

	typedef uint32_t MousButtonCode;

	enum KeyState
	{
		Released,
		Pressed,
		Hold
	};

	class InputSystem
	{
	public:
		void Init();

		void Update();

		void Destroy();

		bool IsKeyPressed(KeyCode keyCode);

		bool IsKeyRelease(KeyCode keyCode);

		bool IsKeyHeld(KeyCode keyCode);

		bool IsMouseButtonPressed(MousButtonCode mouseButton);

		bool IsMouseButtonRelease(MousButtonCode mouseButton);

		bool IsMouseButtonHeld(MousButtonCode mouseButton);

		// MouseScroll 

		void KeyCallBack(KeyCode keyCode, KeyState action);

		void MouseButtonCallBack(MousButtonCode mouseButtonCode, KeyState action);

		void SetMousePosition(const double x, const double y);

		glm::vec2 GetMousePosition() const;

		glm::vec2 GetMouseDelta() const;

		void MouseMove(const double x, const double y);

	private:
		std::unordered_map<KeyCode, KeyState> KeyStateArray;

		std::unordered_map<KeyCode, KeyState> mouseButtonStateArray;

		glm::vec2 mousePosition;

		glm::vec2 mouseDelta {glm::vec2(0.0)};
	};


}
