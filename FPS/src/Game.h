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
#include <Components/TransformComponent.h>
#include <Components/HPComponent.h>
#include <Systems/RenderSystem/RenderSystem.h>
#include <Systems/KeyPressingSystem.h>
#include <Systems/MouseMovingSystem.h>
#include <Systems/GUISystem.h>
#include <Systems/MovementSystem.h>
#include <Systems/RecoilSystem.h>
#include <Systems/PlayerActionSystem.h>
#include <Systems/ParticleSystem.h>
#include <Systems/CollisionSystem.h>
#include <Systems/HitProcessingSystem.h>
#include <Systems/TextSystem.h>
#include <Systems/MonsterCreationSystem.h>


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
		PathRemoveFileSpec(exeFullPath); PathRemoveFileSpec(exeFullPath);  // remove bin/FPS.exe
		gameRootPath = (string)exeFullPath;
		std::replace(gameRootPath.begin(), gameRootPath.end(), '\\', '/');

        // Get the data
		Resource resource = Resource(gameRootPath + "/resources/");
		Resource::SkyBoxResource skybox_resource;
		skybox_resource.init();
		
		// Systems
		world->registerSystem(new KeyPressingSystem());
		world->registerSystem(new MouseMovingSystem());
		world->registerSystem(new RecoilSystem());
		world->registerSystem(new PlayerActionSystem());
        world->registerSystem(new MovementSystem());
        world->registerSystem(new CollisionSystem()); // Must place after movement system
        world->registerSystem(new HitProcessingSystem());
        world->registerSystem(new ParticleSystem());
		world->registerSystem(new RenderSystem(gameRootPath + "/src/Shaders/"));
		world->registerSystem(new GUISystem());  // Must place after render system
		world->registerSystem(new TextSystem());


		// Singleton components
		world->createSingletonComponent<LightingInfoSingletonComponent>();
		world->createSingletonComponent<WindowInfoSingletonComponent>(window, gameRootPath);
		world->createSingletonComponent<SkyboxInfoSingletonComponent>(skybox_resource.vertices, skybox_resource.indices, skybox_resource.textures);

		// Entities
		Entity* player = world->create();
		
		Entity* bullet_text = world->create();
		Entity* hp_text = world->create();
		Entity* test_post = world->create();  // ä»¥åŽ post èµ‹ç»™ gun çš?entityï¼ŒçŽ°åœ¨åªæ˜¯æµ‹è¯?
		Entity* old_man = world->create();

		Entity* hitParticles = world->create();
		Entity* gunFire = world->create();

		Entity* disappear = world->create();	// æ€ªç‰©æ¶ˆå¤±çš„ç²’å­æ•ˆæž?
		Entity* ground = world->create();

		// Initialize static resources of Particle Component
		Resource::ParticleResource particleResource;
		particleResource.init(gameRootPath + "/resources/textures/");

		// Load texture resource
		Resource::TextureResource textureResource;
		textureResource.init();

		// Assign the components to entities
		Resource::CubeResource box_resource, ground_resource;
        Resource::PBR_CubeResource ground_pbr_resource, wall_pbr_resource;

		
		for (int i = 0; i < 5; i++) {
			Entity* box = world->create();
			if (i == 0) {
				box_resource.init(10.0f, 20.0f, 10.0f, textureResource.container_diffuse, textureResource.container_specular);
				box->assign<ObjectComponent>(box_resource.vertices, box_resource.indices, box_resource.textures);
				box->assign<PositionComponent>(glm::vec3(0.0f, 10.0f, 0.0f));
				box->assign<CollisionComponent>(10.0f, 30.0f, 10.0f);
			}
			else {
				float x = 0.0f, z = 0.0f;
				if (i == 1) x = 20.0f;
				if (i == 2) x = -20.0f;
				if (i == 3) z = 20.0f;
				if (i == 4) z = -20.0f;

				box_resource.init(10.0f, 10.0f, 10.0f, textureResource.container_diffuse, textureResource.container_specular);
				box->assign<ObjectComponent>(box_resource.vertices, box_resource.indices, box_resource.textures);
				box->assign<PositionComponent>(glm::vec3(x, 5.0f, z));
				box->assign<CollisionComponent>(10.0f, 10.0f, 10.0f);
			}
		}

		float square = 30.0f, thick = 5.0f, away = 150.0f, height = 30.0f;
		for (int i = 0; i < 8; i++) {
			Entity* wall = world->create();
			float x, z, pos_x, pos_z;
			
			if (i == 0) {
				x = thick; z = away;
				pos_z = square + away / 2; pos_x = square;
			}
			if (i == 1) {
				x = thick; z = away;
				pos_z = square + away / 2; pos_x = -square;
			}
			if (i == 2) {
				x = thick; z = away;
				pos_z = -(square + away / 2); pos_x = square;
			}
			if (i == 3) {
				x = thick; z = away;
				pos_z = -(square + away / 2); pos_x = -square;
			}
			if (i == 4) {
				x = away; z = thick;
				pos_z = square; pos_x = square + away / 2;
			}
			if (i == 5) {
				x = away; z = thick;
				pos_z = square; pos_x = -(square + away / 2);
			}
			if (i == 6) {
				x = away; z = thick;
				pos_z = -square; pos_x = square + away / 2;
			}
			if (i == 7) {
				x = away; z = thick;
				pos_z = -square; pos_x = -(square + away / 2);
			}

			wall_pbr_resource.init(x, height, z, 1.0f,
				textureResource.wall_albedo,
				textureResource.wall_normal,
				textureResource.wall_metallic,
				textureResource.wall_roughness,
				textureResource.wall_ao,
				textureResource.wall_height);

			wall->assign<ObjectComponent>(wall_pbr_resource.vertices, wall_pbr_resource.indices, wall_pbr_resource.textures, "wall", true);
			wall->assign<PositionComponent>(glm::vec3(pos_x, height / 2, pos_z));
			wall->assign<CollisionComponent>(x, height, z);
		}
		Resource::ModelResource monster_resource(gameRootPath + "/resources/objects/Etin/Etin.obj");
		world->registerSystem(new MonsterCreationSystem(monster_resource, square, away));


		old_man->assign<BoneObjectComponent>(gameRootPath + "/resources/bone/boblampclean.md5mesh");
		old_man->assign<PositionComponent>(glm::vec3(0.0f, 80.0f, 0.0f));
		old_man->assign<TransformComponent>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), 0.0f, 0.0f);
		old_man->assign<HPComponent>();

        ground_pbr_resource.init(500.0f, 1.0f, 500.0f, 10.0f,
            textureResource.ground_albedo,
            textureResource.ground_normal,
            textureResource.ground_metallic,
            textureResource.ground_roughness,
            textureResource.ground_ao,
            textureResource.ground_height);
        ground->assign<ObjectComponent>(ground_pbr_resource.vertices, ground_pbr_resource.indices, ground_pbr_resource.textures, "ground", true);
        ground->assign<PositionComponent>(glm::vec3(0.0f, 0.0f, 0.0f));
        ground->assign<CollisionComponent>(500.0f, 1.0f, 500.0f);

		player->assign<ObjectComponent>(gameRootPath + "/resources/objects/gun/Ak-74.obj", "player");

		player->assign<PositionComponent>(glm::vec3(15.0f, 0.6f, 0.0f));
		player->assign<MovementComponent>(glm::vec3(0.0f, -0.1f, 0.0f), glm::vec3(0.0f, -60.0f, 0.0f));  // ç¢°æ’žæ£€æµ‹éœ€è¦ï¼Œè¦ç»™ä¸ªå°ä¸€ç‚¹å‘ä¸‹çš„åˆé€Ÿåº¦ï¼›é¿å…ä¸€å¼€å§‹æ£€æµ‹ä¸åˆ°ç¢°æ’žæŽ‰ä¸‹åŽ»

		player->assign<PlayerComponent>();
		player->assign<TransformComponent>(glm::vec3(-0.63f, 4.52f, 2.0f), glm::vec3(0.022f, 0.022f,0.022f), 0.0f, 180.0f);
		player->assign<CameraComponent>(glm::vec3(0.0f, 5.0f, 0.0f));
        player->assign<CollisionComponent>(-4.0f, 4.0f, 0.0f, 16.0f, -1.5f, 1.5f);

		bullet_text->assign<TextComponent>("bullet_info", "30 / 30", 20.0f, 30.0f, 0.8f, glm::vec3(0.5, 0.8f, 0.2f), gameRootPath + "/resources/fonts/");
		hp_text->assign<TextComponent>("score", "score: ", 600.0f, 30.0f, 0.8f, glm::vec3(0.5, 0.8f, 0.2f), gameRootPath + "/resources/fonts/");

		test_post->assign<PostComponent>(glm::vec3(0.0f, 1.0f, 0.0f), 0.025f);

		hitParticles->assign<ParticleComponent>(particleResource, 1);
		hitParticles->assign<PositionComponent>(glm::vec3());

		gunFire->assign<ParticleComponent>(particleResource, 2);
		gunFire->assign<PositionComponent>(glm::vec3());

		disappear->assign<ParticleComponent>(particleResource, 3);
		disappear->assign<PositionComponent>(glm::vec3());
	}
};
