#pragma once

#include <ECS.h>
#include <glm/gtc/type_ptr.hpp>

using namespace ECS;

// 单例组件；存储全局的光照位置，颜色以及渲染参数
struct LightingInfoSingletonComponent {

	glm::vec3 LightPos;
	glm::vec3 LightColor;

	float AmbientStrength, SpecularStrength, DiffuseStrength, Shininess;
	float bias;

	GLuint depthMapFBO;
	GLuint depthMap;
	GLuint shadow_width, shadow_height;
	glm::mat4 lightSpaceMatrix;

	LightingInfoSingletonComponent() {
		LightPos = glm::vec3(10.0f, 50.0f, 10.0f);
		LightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		AmbientStrength = 0.5f;
		SpecularStrength = 0.15f;
		DiffuseStrength = 0.75f;
		Shininess = 32.0f;

		init();
	}

	void init() {
		shadow_width = shadow_height = 4096;
		glGenFramebuffers(1, &depthMapFBO);
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};

