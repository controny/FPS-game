#pragma once
#include <algorithm>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include <Game.h>

#include <ECS.h>
#include <Components/ObjectComponent.h>
#include <Components/PositionComponent.h>
#include <Components/TextComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Components/SkyboxInfoSingletonComponent.h>
#include <Components/ParticleComponent.h>
#include <cmath>

using namespace ECS;

// 获取所有需要渲染的组件件并渲染
class RenderSystem : public EntitySystem {
public:

	Shader objectShader;
	Shader textShader;
	Shader skyboxShader;
	Shader postShader;
	Shader boneShader;
	Shader particleShader;
	Shader depthShader;
	Shader boneDepthShader;


	RenderSystem(string shader_dir) {
		objectShader.init("object.vs", "object.fs", shader_dir);
		textShader.init("text.vs", "text.fs", shader_dir);
		skyboxShader.init("skybox.vs", "skybox.fs", shader_dir);
		postShader.init("post.vs", "post.fs", shader_dir);
		boneShader.init("skinning.vs", "object.fs", shader_dir);
		particleShader.init("particle.vs", "particle.fs", shader_dir);
		depthShader.init("depth.vs", "depth.fs", shader_dir);
		boneDepthShader.init("boneDepth.vs", "depth.fs", shader_dir);
	}

	virtual void tick(class World* world, float deltaTime) override
	{
		renderDepth(world, deltaTime);
		render(world, deltaTime);
	}

private:
	void renderDepth(class World* world, float deltaTime) {
		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);

		glm::mat4 lightProjection, lightView;

		GLfloat near_plane = 1.0f, far_plane = 75.0f;

		//lightProjection = glm::perspective(glm::radians(89.0f), (float)window_width / (float)window_height, near_plane, far_plane);
		lightProjection = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightCHandle->LightPos, glm::vec3(-4.0f, 0.0f, -4.0f), glm::vec3(0.0, 1.0, 0.0));

		lightCHandle->lightSpaceMatrix = lightProjection * lightView;

		glViewport(0, 0, lightCHandle->shadow_width, lightCHandle->shadow_height);
		glBindFramebuffer(GL_FRAMEBUFFER, lightCHandle->depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightCHandle->lightSpaceMatrix);
		renderMeshes(world, deltaTime, depthShader);

		boneDepthShader.use();
		boneDepthShader.setMat4("lightSpaceMatrix", lightCHandle->lightSpaceMatrix);
		renderBoneObject(world, deltaTime, boneDepthShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void render(class World* world, float deltaTime) {
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();

		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 ViewMatrix;
		glm::vec3 CameraPos;
		world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {
			auto cameraCHandle = ent->get<CameraComponent>();
			auto positionCHandle = ent->get<PositionComponent>();

			glm::vec3 XZ_front = glm::normalize(glm::vec3(positionCHandle->Front.x, 0.0f, positionCHandle->Front.z));
			CameraPos = positionCHandle->Position + glm::vec3(XZ_front.x, cameraCHandle->Relative_position.y, XZ_front.z);

			ViewMatrix = glm::lookAt(CameraPos, CameraPos + positionCHandle->Front, positionCHandle->Up);
			// 计算出位置后存在里面，这样在其他地方就不用再算一次
			cameraCHandle->Position = CameraPos;
		});

		/* ----------- render object -----------*/
		objectShader.use();

		// 设置着色器要用的变量
		objectShader.setMat4("view", ViewMatrix);
		objectShader.setVec3("viewPos", CameraPos);

		objectShader.setVec3("lightDirection", lightCHandle->LightDirection);
		objectShader.setVec3("lightColor", lightCHandle->LightColor);
		objectShader.setFloat("ambientStrength", lightCHandle->AmbientStrength);
		objectShader.setFloat("specularStrength", lightCHandle->SpecularStrength);
		objectShader.setFloat("shininess", lightCHandle->Shininess);
		objectShader.setFloat("diffuseStrength", lightCHandle->DiffuseStrength);

		glm::mat4 projection = glm::perspective(45.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("lightSpaceMatrix", lightCHandle->lightSpaceMatrix);
		objectShader.setInt("shadow_type", lightCHandle->shadow_type);
			
		renderMeshes(world, deltaTime, objectShader);


		//设置骨骼着色器变量
		glm::mat4 bonemodel = glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 0.1f));

		boneShader.use();
		boneShader.setMat4("view", ViewMatrix);
		boneShader.setVec3("viewPos", CameraPos);

		boneShader.setVec3("lightDirection", lightCHandle->LightDirection);
		boneShader.setVec3("lightColor", lightCHandle->LightColor);
		boneShader.setFloat("ambientStrength", lightCHandle->AmbientStrength);
		boneShader.setFloat("specularStrength", lightCHandle->SpecularStrength);
		boneShader.setFloat("shininess", lightCHandle->Shininess);
		boneShader.setFloat("diffuseStrength", lightCHandle->DiffuseStrength);

		bonemodel = glm::rotate(bonemodel, 180.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		boneShader.setMat4("model", bonemodel);
		boneShader.setMat4("projection", projection);
		boneShader.setInt("shadow_type", lightCHandle->shadow_type);

		
		
		renderBoneObject(world, deltaTime, boneShader);

		/* ----------- render skybox -----------*/
		skyboxShader.use();

		//设置天空盒着色器变量
		glm::mat4 view = glm::mat4(glm::mat3(ViewMatrix));
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);

		renderSkybox(world, deltaTime);


		/* ----------- render text -----------*/
		textShader.use();
		projection = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -0.001f, 1.0f);
		textShader.setMat4("projection", projection);

		renderText(world, deltaTime);


		/* ----------- render post -----------*/
		postShader.use();
		renderPost(world, deltaTime);

		/* ----------- render particles -----------*/
		renderParticles(world, deltaTime, ViewMatrix);
	}

	void renderMeshes(class World* world, float deltaTime, Shader shader) {

		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();

		// 渲染，就是之前 Mesh 类的 Draw()
		world->each<ObjectComponent, PositionComponent>(
			[&](Entity* ent,
				ComponentHandle<ObjectComponent> objectCHandle,
				ComponentHandle<PositionComponent> positionCHandle) -> void {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			vector<Mesh>& meshes = objectCHandle->meshes;
			for (unsigned int j = 0; j < meshes.size(); j++) {
				Mesh& mesh = meshes[j];
				int i = 0;
				for (i = 0; i < mesh.textures.size(); i++)
				{
					glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
													  // retrieve texture number (the N in diffuse_textureN)
					string number;
					string name = mesh.textures[i].type;
					if (name == "texture_diffuse")
						number = std::to_string(diffuseNr++);
					else if (name == "texture_specular")
						number = std::to_string(specularNr++); // transfer unsigned int to stream
					else if (name == "texture_normal")
						number = std::to_string(normalNr++); // transfer unsigned int to stream
					else if (name == "texture_height")
						number = std::to_string(heightNr++); // transfer unsigned int to stream

															 // now set the sampler to the correct texture unit
					int a = glGetUniformLocation(shader.ID, (name + number).c_str());
					glUniform1i(a, i);

					// and finally bind the texture
					glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
				}

				glActiveTexture(GL_TEXTURE0 + i);
				shader.setInt("shadowMap", i);
				glBindTexture(GL_TEXTURE_2D, lightCHandle->depthMap);

				glm::mat4 model;
				model = glm::translate(model, positionCHandle->Position);


				glm::vec3 XZ_front = glm::normalize(glm::vec3(positionCHandle->Front.x, 0.0f, positionCHandle->Front.z));
				float x = XZ_front.x, z = XZ_front.z;
				
				// 根据 Front、Right 向量对 player 的模型进行旋转
				if (x > 0 && z > 0) {
					model = glm::rotate(model, glm::acos(z), glm::vec3(0.0, 1.0, 0.0));
				}
				else if (x > 0 && z < 0) {
					model = glm::rotate(model, float(glm::acos(z)), glm::vec3(0.0, 1.0, 0.0));
				}
				else if (x < 0 && z > 0) {
					model = glm::rotate(model, float(-glm::acos(z)), glm::vec3(0.0, 1.0, 0.0));
				}
				else if (x < 0 && z < 0) {
					model = glm::rotate(model, float(6.28 - glm::acos(z)), glm::vec3(0.0, 1.0, 0.0));
				}

				shader.setMat4("model", model);
				//glDepthFunc(GL_LEQUAL);
				glBindVertexArray(mesh.VAO);
				glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				//glDepthFunc(GL_LESS);
				//glActiveTexture(GL_TEXTURE0);
			}
		});
	}



	void renderBoneObject(class World* world, float deltaTime, Shader shader) {
		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();

		glm::mat4 bonemodel = glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 0.1f));
		bonemodel = glm::rotate(bonemodel, 180.0f, glm::vec3(1.0f, 1.0f, 1.0f));

		shader.setMat4("model", bonemodel);

		// 渲染骨骼模型
		world->each<BoneObjectComponent>([&](Entity* ent, ComponentHandle<BoneObjectComponent> BoneobjectCHandle) -> void {
			static vector<pair<string, const aiScene*> > loadedScenes;
			bool hasLoaded = false;
			for (int i = 0; i < loadedScenes.size(); ++i) {
				if (loadedScenes[i].first == BoneobjectCHandle->filename) {
					hasLoaded = true;
					BoneobjectCHandle->m_pScene = loadedScenes[i].second;
					break;
				}
			}
			if (!hasLoaded) {
				
				string file = BoneobjectCHandle->filename;
				const aiScene* scene = BoneobjectCHandle->m_Importer.ReadFile(BoneobjectCHandle->filename, ASSIMP_LOAD_FLAGS);
				BoneobjectCHandle->m_pScene = scene;
				pair<string, const aiScene*> p(file, scene);
				loadedScenes.push_back(p);
			}
			
			static vector<Matrix4f> Transforms;
			//float RunningTime = GetRunningTime();
			static float RunningTime = 0.5;
			RunningTime += 0.1;
			BoneobjectCHandle->BoneTransform(RunningTime, Transforms);
			GLuint m_boneLocation[100];
			for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
				char Name[128];
				memset(Name, 0, sizeof(Name));
				SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
				m_boneLocation[i] = glGetUniformLocation(shader.ID, Name);
			}
			for (uint i = 0; i < Transforms.size(); i++) {
				//m_pEffect->SetBoneTransform(i, Transforms[i]);
				glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)Transforms[i]);
			}
			
			glActiveTexture(GL_TEXTURE15);
			boneShader.setInt("shadowMap", 15);
			glBindTexture(GL_TEXTURE_2D, lightCHandle->depthMap);
			
			BoneobjectCHandle->Render();
		});
	}
	void renderSkybox(class World* world, float deltaTime) {
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		auto SkyboxCHandle = world->getSingletonComponent<SkyboxInfoSingletonComponent>();
		vector<Mesh>& meshes = SkyboxCHandle->meshes;
		for (unsigned int j = 0; j < meshes.size(); j++) {
			Mesh& mesh = meshes[j];
			for (unsigned int i = 0; i < mesh.textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
												  // retrieve texture number (the N in diffuse_textureN)
				string number;
				string name = mesh.textures[i].type;
				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++); // transfer unsigned int to stream
				else if (name == "texture_normal")
					number = std::to_string(normalNr++); // transfer unsigned int to stream
				else if (name == "texture_height")
					number = std::to_string(heightNr++); // transfer unsigned int to stream

														 // now set the sampler to the correct texture unit
														 //设置天空盒变量
				int b = glGetUniformLocation(skyboxShader.ID, (name + number).c_str());
				glUniform1i(b, i);

				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
			}
			glDepthFunc(GL_LEQUAL);
			glBindVertexArray(mesh.VAO);
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS);
			glActiveTexture(GL_TEXTURE0);
		}
	}

	void renderText(class World* world, float deltaTime) {
		world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) {
			textShader.setVec3("textColor", textCHandle->color);
			glActiveTexture(GL_TEXTURE0);

			for (int i = 0; i < textCHandle->VAOs.size(); i++) {
				glBindVertexArray(textCHandle->VAOs[i]);
				glBindTexture(GL_TEXTURE_2D, textCHandle->TextureIDs[i]);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		});
	}

	void renderPost(class World* world, float deltaTime) {
		world->each<PostComponent>([&](Entity* ent, ComponentHandle<PostComponent> postCHandle) {
			postShader.setVec3("color", postCHandle->Color);
			glBindVertexArray(postCHandle->line_VAO);
			glLineWidth(2);
			glDrawArrays(GL_LINES, 0, 8);
			glBindVertexArray(postCHandle->point_VAO);
			glPointSize(2);
			glDrawArrays(GL_POINTS, 0, 1);
			glBindVertexArray(0);
		});
	}

	void renderParticles(class World* world, float deltaTime, glm::mat4 ViewMatrix) {
		world->each<ParticleComponent>([&](Entity* ent, ComponentHandle<ParticleComponent> particleCHandle) {
			// 达到一定数量后就不再渲染
			if (particleCHandle->producedParticles > particleCHandle->MAX_TOTAL_NUM)
				return;
			// Accept fragment if it closer to the camera than the former one
			glDepthFunc(GL_LESS);

			GLuint VertexArrayID;
			glGenVertexArrays(1, &VertexArrayID);
			glBindVertexArray(VertexArrayID);

			// Update the buffers that OpenGL uses for rendering.
			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->particles_position_buffer);
			glBufferData(GL_ARRAY_BUFFER, particleCHandle->maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
			glBufferSubData(GL_ARRAY_BUFFER, 0, particleCHandle->particlesCount * sizeof(GLfloat) * 4, particleCHandle->g_particule_position_size_data);

			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->particles_color_buffer);
			glBufferData(GL_ARRAY_BUFFER, particleCHandle->maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
			glBufferSubData(GL_ARRAY_BUFFER, 0, particleCHandle->particlesCount * sizeof(GLubyte) * 4, particleCHandle->g_particule_color_data);


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Use our shader
			particleShader.use();

			// Bind our texture in Texture Unit 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, particleCHandle->texture);
			// Set our "myTextureSampler" sampler to use Texture Unit 0
			particleShader.setInt("myTextureSampler", 0);

			// Same as the billboards tutorial
			//glm::mat4 viewMatrix = ViewMatrix;
			// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 viewProjectionMatrix = projectionMatrix * ViewMatrix;
			particleShader.setVec3("CameraRight_worldspace",
				ViewMatrix[0][0],
				ViewMatrix[1][0],
				ViewMatrix[2][0]);
			particleShader.setVec3("CameraUp_worldspace",
				ViewMatrix[0][1],
				ViewMatrix[1][1],
				ViewMatrix[2][1]);
			particleShader.setMat4("VP", viewProjectionMatrix);

			// 1rst attribute buffer : vertices
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->billboard_vertex_buffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : positions of particles' centers
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->particles_position_buffer);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				4,                                // size : x + y + z + size => 4
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// 3rd attribute buffer : particles' colors
			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->particles_color_buffer);
			glVertexAttribPointer(
				2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				4,                                // size : r + g + b + a => 4
				GL_UNSIGNED_BYTE,                 // type
				GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			// These functions are specific to glDrawArrays*Instanced*.
			// The first parameter is the attribute buffer we're talking about.
			// The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
			// http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
			glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
			glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
			glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

										 // Draw the particules !
										 // This draws many times a small triangle_strip (which looks like a quad).
										 // This is equivalent to :
										 // for(i in particleCHandle->particlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
										 // but faster.
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCHandle->particlesCount);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		});
	}
};
