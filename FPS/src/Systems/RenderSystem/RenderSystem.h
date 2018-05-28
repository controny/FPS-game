#pragma once
#include <algorithm>
#include <GLFW/glfw3.h>

#include "Shader.h"

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
	Shader particleShader;

	RenderSystem() {
		objectShader.init("src/Shaders/object.vs", "src/Shaders/object.fs");
		textShader.init("src/Shaders/text.vs", "src/Shaders/text.fs");
		skyboxShader.init("src/Shaders/skybox.vs", "src/Shaders/skybox.fs");
		postShader.init("src/Shaders/post.vs", "src/Shaders/post.fs");
		particleShader.init("src/Shaders/particle.vs", "src/Shaders/particle.fs");
	}

	virtual void tick(class World* world, float deltaTime) override
	{
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();
		
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

		objectShader.setVec3("lightPos", lightCHandle->LightPos);
		objectShader.setVec3("lightColor", lightCHandle->LightColor);
		objectShader.setFloat("ambientStrength", lightCHandle->AmbientStrength);
		objectShader.setFloat("specularStrength", lightCHandle->SpecularStrength);
		objectShader.setFloat("shininess", lightCHandle->Shininess);
		objectShader.setFloat("diffuseStrength", lightCHandle->DiffuseStrength);

		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);

		glm::mat4 projection = glm::perspective(45.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
		objectShader.setMat4("projection", projection);

		renderMeshes(world, deltaTime);


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
private:
	void renderMeshes(class World* world, float deltaTime) {

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
					int a = glGetUniformLocation(objectShader.ID, (name + number).c_str());
					glUniform1i(a, i);

					// and finally bind the texture
					glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
				}
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

				objectShader.setMat4("model", model);
				glDepthFunc(GL_LEQUAL);
				glBindVertexArray(mesh.VAO);
				glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS);
				glActiveTexture(GL_TEXTURE0);
			}
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
