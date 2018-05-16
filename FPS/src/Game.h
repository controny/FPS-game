#pragma once
#include <glad/glad.h>
#include <iostream>
#include <ECS.h>

#include "Resource.h"

#include <Components/MeshComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Systems/RenderSystem.h>
#include <Systems/CameraMovingSystem.h>
#include <Systems/KeyPressSystem.h>


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
		/* window bindings */
		glfwSetCursorPosCallback(window, mouse_callback);

		glfwGetWindowSize(window, &window_width, &window_height);

		lastX = window_width / 2.0f;
		lastY = window_height / 2.0f;

		/* world bindings */
		
		// Get the data
		Resource resource = Resource();
		
		// Systems
		world->registerSystem(new RenderSystem());
		world->registerSystem(new KeyPressSystem());
		world->registerSystem(new CameraMovingSystem(cameraPos));

		// Entities
		Entity* singletons = world->create();
		singletons->assign<CameraInfoSingletonComponent>(cameraPos);
		singletons->assign<LightingInfoSingletonComponent>();
		singletons->assign<WindowInfoSingletonComponent>(window_width, window_height, window);

		Entity* cube1 = world->create();
		Entity* cube2 = world->create();

		// Assign the components to entities
		Resource::CubeResource cube_resource, cube_resource1;
		cube_resource.init(glm::vec3(-1.5f, 0.5f, -1.5f), 1.0f);
		cube1->assign<MeshComponent>(cube_resource.vertices, cube_resource.indices, cube_resource.textures);
		cube_resource1.init(glm::vec3(1.5f, -0.5f, 1.5f), 1.0f);
		cube2->assign<MeshComponent>(cube_resource1.vertices, cube_resource1.indices, cube_resource1.textures);
	}


	// 因为需要注册到 window 上，而且只需要注册一次，所以这部分逻辑就写在了外面，没有弄进 system 中
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

		world->emit<MouseMovementEvent>({xoffset ,yoffset});
	}
};


