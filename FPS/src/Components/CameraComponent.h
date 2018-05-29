#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

// 之前的 Camera.h 中的数据。Front、Right、Up 三个向量分离 PositionComponent 中；视角更新逻辑再 MouseMovingSystem中；camera 不再需要考虑移动，每次根据绑定的 entity 的位置来更新位置
struct CameraComponent {

	// Camera Attributes
	glm::vec3 Position;  // update by relative position
	glm::vec3 Relative_position;  // the position relative to entity's position component

	// Euler Angles
	float Yaw;
	float Pitch;

	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	// Constructor with vectors
	CameraComponent(glm::vec3 _relative_position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Relative_position = _relative_position;
		Yaw = yaw;
		Pitch = pitch;
	}
};