#pragma once

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

using namespace ECS;

// 单例组件；存储全局的 Camera 的位置和视角信息
struct CameraInfoSingletonComponent {

	glm::mat4 CameraViewMatrix;
	glm::vec3 CameraPos;

	CameraInfoSingletonComponent(glm::vec3 _cameraPos) {
		CameraPos = _cameraPos;
	}
};
