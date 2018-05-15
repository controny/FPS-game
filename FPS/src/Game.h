#pragma once
#include <glad/glad.h>
#include <iostream>
#include <ECS.h>

#include "Resource.h"

#include "MeshComponent.h"
#include "Components\CameraInfoSingletonComponent.h"
#include "Components\LightingInfoSingletonComponent.h"
#include "Components\WindowInfoSingletonComponent.h"
#include "Systems/RenderSystem.h"
#include "Systems\CameraMovingSystem.h"


namespace Game {

	World* world = World::createWorld();
	GLFWwindow* window;

	int window_width = 0;
	int window_height = 0;

	glm::vec3 cameraPos(0.0f, 3.0f, 5.0f);

	bool firstMouse = true;
	float lastX, lastY;

	void mouse_callback(GLFWwindow* window, double xpos, double ypos);


	void init() {
		// window bindings
		glfwSetCursorPosCallback(window, mouse_callback);

		glfwGetWindowSize(window, &window_width, &window_height);

		lastX = window_width / 2.0f;
		lastY = window_height / 2.0f;

		// world bindings
		Resource resource = Resource();

		world->registerSystem(new RenderSystem());
		world->registerSystem(new CameraMovingSystem(cameraPos));

		Entity* singletons = world->create();
		singletons->assign<CameraInfoSingletonComponent>(cameraPos);
		singletons->assign<LightingInfoSingletonComponent>();
		singletons->assign<WindowInfoSingletonComponent>(window_width, window_height, window);

		Entity* cube1 = world->create();
		Entity* cube2 = world->create();


		Resource::CubeResource cube_resource, cube_resource1;
		cube_resource.init(glm::vec3(-1.5f, 0.5f, -1.5f), 1.0f);
		cube1->assign<MeshComponent>(cube_resource.vertices, cube_resource.indices, cube_resource.textures);
		cube_resource1.init(glm::vec3(1.5f, -0.5f, 1.5f), 1.0f);
		cube2->assign<MeshComponent>(cube_resource1.vertices, cube_resource1.indices, cube_resource1.textures);
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		world->emit<MouseMovementEvent>({ xoffset ,yoffset });
	}
};


