#pragma once

//#include "glm/glm.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Reinkan::Camera
{
	enum class CameraStateFlag 
	{
		None = 0x00,
		ToggleOrbit = 0x01, // for spin in first person
		TogglePan = 0x02
	};

	class Camera
	{
	public:
		Camera();

		Camera(glm::vec3& position,
				float spin,
				float tilt,
				float fieldOfView,
				float pFront,
				float pBack,
				float pRy,
				float speed,
				float rotateSpeed);

		CameraStateFlag cameraStateFlag = CameraStateFlag::None;

		void UpdatePerspectiveMatrix(const float aspect);

		void UpdateViewMatrix(const float time);

		void UpdateDirection();

		glm::vec3 GetPosition() const;

		void SetPosition(glm::vec3 position);

		glm::mat4 GetPerspectiveMatrix() const;

		glm::mat4 GetViewMatrix() const;

		void Move(const glm::vec3& distance);

		void Spin(float spinRad);

		void Tilt(float tiltRad);

		void UpdateCursorPosition(const float x, const float y);

		float GetSpeed() const;

		float GetRotateSpeed() const;

		void SetDirection(glm::vec3 direction);

		glm::vec3 GetDirection() const;

		glm::vec3 GetUp() const;

		void SetSpin(float spin);

		void SetTilt(float tilt);

		float GetSpin() const;

		float GetTilt() const;

		float GetNearPlane() const;

		float GetFarPlane() const;

	private:
		glm::vec3 position = glm::vec3(0);

		float spin = 0.0f;

		float tilt = 0.0f;

		glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 1.0);

		glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);

		glm::mat4 perspectiveMatrix;

		glm::mat4 viewMatrix;

		float cursorPosX = 0.0f;

		float cursorPosY = 0.0f;

		float fieldOfView = 45.0f;

		float pFront = 0.1f;

		float pBack = 100.0f;

		float rY = 0.57f;

		float speed;

		float rotateSpeed = 0.2f;
	};
}