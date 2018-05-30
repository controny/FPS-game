#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include <Winbase.h>
#include <Shlwapi.h>


#include <Resource.h>
#include <ECS.h>
#include <Components/CollisionComponent.h>
#include <Components/MovementComponent.h>
#include <Components/PositionComponent.h>
#include <Components/TextComponent.h>
#include <Components/ObjectComponent.h>
#include <Components/TextComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Components/SkyboxInfoSingletonComponent.h>
#include <Components/PostComponent.h>
#include <Components/BoneObjectComponent.h>
#include <Components/PlayerComponent.h>
#include <Components/CameraComponent.h>
#include <Systems/RenderSystem/RenderSystem.h>
#include <Systems/KeyPressingSystem.h>
#include <Systems/MouseMovingSystem.h>
#include <Systems/GUISystem.h>
#include <Systems/MovementSystem.h>
#include <Systems/RecoilSystem.h>
#include <Systems/PlayerActionSystem.h>
#include <Systems/ParticleSystem.h>
#include <Systems/CollisionSystem.h>


class Game {
public:
	string gameRootPath;

	World* world;
	GLFWwindow* window;

	Game(GLFWwindow* _window) {
		init(_window);
	}

	void init(GLFWwindow* _window) {
		world = World::createWorld();
		window = _window;

		char exeFullPath[MAX_PATH]; // Full path
		GetModuleFileName(NULL, exeFullPath, MAX_PATH);
		PathRemoveFileSpec(exeFullPath);
		PathRemoveFileSpec(exeFullPath);  // remove bin/FPS.exe
		gameRootPath = (string)exeFullPath;
		std::replace(gameRootPath.begin(), gameRootPath.end(), '\\', '/');

        // Get the data
		Resource resource = Resource(gameRootPath + "/resources/");
		Resource::SkyBoxResource skybox_resource;
		skybox_resource.init();
		
		// Systems
		world->registerSystem(new KeyPressingSystem());
		world->registerSystem(new MouseMovingSystem());
		//world->registerSystem(new CameraMovingSystem(cameraPos));
		world->registerSystem(new RecoilSystem());
		world->registerSystem(new PlayerActionSystem());
        world->registerSystem(new MovementSystem());
        world->registerSystem(new CollisionSystem()); // Must place after movement system
        world->registerSystem(new ParticleSystem());
		world->registerSystem(new RenderSystem(gameRootPath + "/src/Shaders/"));
		world->registerSystem(new GUISystem());  // Must place after render system


		// Singleton components
		world->createSingletonComponent<LightingInfoSingletonComponent>();
		world->createSingletonComponent<WindowInfoSingletonComponent>(window);
		world->createSingletonComponent<SkyboxInfoSingletonComponent>(skybox_resource.vertices, skybox_resource.indices, skybox_resource.textures);

		// Entities
		Entity* wall = world->create();
		Entity* player = world->create();
		Entity* ground = world->create();
		Entity* text = world->create();
		Entity* test_post = world->create();  // 以后 post 赋给 gun 的 entity，现在只是测试
		Entity* skeleton_model = world->create();
		Entity* particles = world->create();

		// Load texture resource
		Resource::TextureResource textureResource;
		textureResource.init();

		// Assign the components to entities
		Resource::CubeResource wall_resource, ground_resource;
        wall_resource.init(20.0f, 20.0f, 10.0f, textureResource.container_diffuse, textureResource.container_specular);
        wall->assign<ObjectComponent>(wall_resource.vertices, wall_resource.indices, wall_resource.textures);
        wall->assign<PositionComponent>(glm::vec3(30.0f, 5.0f, 0.0f));
        wall->assign<CollisionComponent>(20.0f, 20.0f, 10.0f);

		ground_resource.init(500.0f, 500.0f, 1.0f, textureResource.ground_diffuse, textureResource.ground_specular);
		ground->assign<ObjectComponent>(ground_resource.vertices, ground_resource.indices, ground_resource.textures);
        ground->assign<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
        ground->assign<CollisionComponent>(500.0f, 500.0f, 1.0f);

		player->assign<ObjectComponent>(gameRootPath + "/resources/objects/nanosuit/nanosuit.obj");
		player->assign<PositionComponent>(glm::vec3(0.0f, 0.6f, 0.0f));
		player->assign<MovementComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -60.0f, 0.0f));
		player->assign<PlayerComponent>();
		player->assign<CameraComponent>(glm::vec3(0.0f, 14.0f, 1.0f));
        player->assign<CollisionComponent>(-4.0f, 4.0f, 0.0f, 16.0f, -1.5f, 1.5f);

		text->assign<TextComponent>("test", 15.0f, 8.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), gameRootPath + "/resources/fonts/");

		test_post->assign<PostComponent>(glm::vec3(0.0f, 1.0f, 0.0f), 0.025f);
		
		//skeleton_model->assign<BoneObjectComponent>(gameRootPath + "/resources/bone/boblampclean.md5mesh");

		particles->assign<ParticleComponent>(gameRootPath + "/resources/textures/", 500, 5.0f, glm::vec3(0.0f, 8.0f, 0.0f), 128, 1, 1);
        particles->assign<PositionComponent>(glm::vec3(0.0f, 3.0f, -10.0f));
	}
};
