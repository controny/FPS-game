#pragma once

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

using namespace ECS;

// 单例组件；存储全局的光照位置，颜色以及渲染参数
struct LightingInfoSingletonComponent {

	glm::vec3 LightPos;
	glm::vec3 LightColor;

	float AmbientStrength, SpecularStrength, DiffuseStrength, Shininess;

	LightingInfoSingletonComponent() {
		LightPos = glm::vec3(5.0f, 5.0f, 5.0f);
		LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		AmbientStrength = 0.5f;
		SpecularStrength = 1.0f;
		DiffuseStrength = 1.0f;
		Shininess = 32.0f;
	}
};

