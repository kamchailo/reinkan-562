#include "pch.h"
#include "Graphics/ReinkanVulkan.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <cmath>

#include "Core/Locator/TimeSystemLocator.h"
#include "Core/Locator/InputSystemLocator.h"

namespace Reinkan::Graphics
{
	void ReinkanApp::SetMainCamera(Camera::Camera* camera)
	{
		appMainCamera = camera;
	}

	void ReinkanApp::UpdateCamera()
	{
		if (!appMainCamera) return;

		Input::InputSystem* inputSystem = Core::InputSystemLocator::GetInputSystem();

		double deltaTime = Core::TimeSystemLocator::GetTime()->GetDeltaTime();

		float speed = appMainCamera->GetSpeed() * deltaTime;

		float rotateSpeed = appMainCamera->GetRotateSpeed() * deltaTime;

		glm::vec3 direction = appMainCamera->GetDirection();

		glm::vec3 upDirection = appMainCamera->GetUp();

		float rad = 3.14159 / 180;

		if (inputSystem->IsKeyHeld(GLFW_KEY_W))
		{
			appMainCamera->Move(speed * direction);
		}

		if (inputSystem->IsKeyHeld(GLFW_KEY_S))
		{
			appMainCamera->Move(speed * -direction);
		}

		if (inputSystem->IsKeyHeld(GLFW_KEY_A))
		{
			glm::vec3 sideDirection = glm::cross(direction, upDirection);
			appMainCamera->Move(speed * -sideDirection);
		}

		if (inputSystem->IsKeyHeld(GLFW_KEY_D))
		{
			glm::vec3 sideDirection = glm::cross(direction, upDirection);
			appMainCamera->Move(speed * sideDirection);
		}

		if (inputSystem->IsKeyHeld(GLFW_KEY_E))
		{
			appMainCamera->Move(speed * upDirection);
		}
		else if (inputSystem->IsKeyHeld(GLFW_KEY_C))
		{
			appMainCamera->Move(speed * -upDirection);
		}

		if (inputSystem->IsMouseButtonHeld(GLFW_MOUSE_BUTTON_LEFT))
		{
			appMainCamera->Spin(rotateSpeed * inputSystem->GetMouseDelta().x);

			appMainCamera->Tilt(rotateSpeed * inputSystem->GetMouseDelta().y);
		}

		//appMainCamera->UpdateCursorPosition();

		float aspect = static_cast<float>(appSwapchainExtent.width) / static_cast<float>(appSwapchainExtent.height);

		appMainCamera->UpdatePerspectiveMatrix(aspect);

		appMainCamera->UpdateViewMatrix(deltaTime);

		appMainCamera->UpdateDirection();
	}
	Camera::Camera* ReinkanApp::GetMainCamera() const
	{
		return appMainCamera;
	}
}