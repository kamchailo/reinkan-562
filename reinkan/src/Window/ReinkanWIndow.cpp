#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include "Core/Locator/InputSystemLocator.h"
#include "Input/InputSystem.h"

namespace Reinkan::Graphics
{
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto app = reinterpret_cast<ReinkanApp*>(glfwGetWindowUserPointer(window));
		app->appFramebufferResized = true;
	}

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        const bool pressed = action != GLFW_RELEASE;

        if (pressed && key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, 1);
        }

        Input::KeyState state;
        if (action != GLFW_RELEASE)
        {
            state = Input::KeyState::Pressed;
        }
        else
        {
            state = Input::KeyState::Released;
        }

        Core::InputSystemLocator::GetInputSystem()->KeyCallBack(key, state);
    }
    
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
#ifdef GUI
        if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse)
            return;
#endif

        auto inputSystem = Core::InputSystemLocator::GetInputSystem();

        //std::printf("mouse position x: %f, y: %f action: %d Key: %d\n", x, y, action, button);

        Input::KeyState state;
        if (action != GLFW_RELEASE)
        {
            double x;
            double y;
            glfwGetCursorPos(window, &x, &y);
            inputSystem->SetMousePosition(x, y);
            state = Input::KeyState::Pressed;
        }
        else
        {
            state = Input::KeyState::Released;
        }

        inputSystem->MouseButtonCallBack(button, state);
    }

    void CursorPosCallback(GLFWwindow* window, double x, double y)
    {
#ifdef GUI
        if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse)
            return;
#endif

        //if (app->myCamera.lmb || app->myCamera.rmb || app->myCamera.mmb)
            //app->myCamera.mouseMove(x, y);
        auto inputSystem = Core::InputSystemLocator::GetInputSystem();

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) != GLFW_RELEASE)
        {
            inputSystem->MouseMove(x, y);
            //std::printf("mouse move x: %f, y: %f\n", x, y);
        }
    }

    void CharCallback(GLFWwindow* window, unsigned int key)
    {
#ifdef GUI
        if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard)
            return;
#endif
    }
    
    void ScrollCallback(GLFWwindow* window, double x, double y)
    {
#ifdef GUI
        if (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse)
            return;
#endif

        int delta = y;
        //if (delta != 0)
            //app->myCamera.wheel(delta > 0 ? 1 : -1);
    }

    
	void ReinkanApp::InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		appWindow = glfwCreateWindow(windowWidth, windowHeight, "Reinkan", nullptr, nullptr);
		glfwSetWindowUserPointer(appWindow, this);

		glfwSetFramebufferSizeCallback(appWindow, FramebufferResizeCallback);

        glfwSetKeyCallback(appWindow, KeyCallback);
		glfwSetCursorPosCallback(appWindow, &CursorPosCallback);
        glfwSetMouseButtonCallback(appWindow, &MouseButtonCallback);
        glfwSetScrollCallback(appWindow, &ScrollCallback);

	}
}