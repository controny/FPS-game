#pragma once

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

// 单例组件；存储全局的 Camera 的位置和视角信息
struct CameraInfoSingletonComponent {
	ECS_DECLARE_TYPE;

	glm::mat4 CameraViewMatrix;
	glm::vec3 CameraPos;

	CameraInfoSingletonComponent(glm::vec3 _cameraPos) {
		CameraPos = _cameraPos;
	}
};

ECS_DEFINE_TYPE(CameraInfoSingletonComponent);