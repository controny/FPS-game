#pragma once
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <ECS.h>
#include <Components/ObjectComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Components/SkyboxInfoSingletonComponent.h>

using namespace ECS;

// 获取所有的 mesh 组件并渲染
class RenderSystem : public EntitySystem {
public:

	Shader objectShader;
	Shader skyboxShader;

	RenderSystem() {
		objectShader.init("src/Shaders/object.vs", "src/Shaders/object.fs");
		skyboxShader.init("src/Shaders/skybox.vs", "src/Shaders/skybox.fs");
	}

	virtual void tick(class World* world, float deltaTime) override
	{
		auto cameraCHandle = world->getSingletonComponent<CameraInfoSingletonComponent>();
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();
		auto SkyboxCHandle = world->getSingletonComponent<SkyboxInfoSingletonComponent>();


		objectShader.use();

		// 设置着色器要用的变量
		objectShader.setMat4("view", cameraCHandle->CameraViewMatrix);
		objectShader.setVec3("viewPos", cameraCHandle->CameraPos);

		objectShader.setVec3("lightPos", lightCHandle->LightPos);
		objectShader.setVec3("lightColor", lightCHandle->LightColor);
		objectShader.setFloat("ambientStrength", lightCHandle->AmbientStrength);
		objectShader.setFloat("specularStrength", lightCHandle->SpecularStrength);
		objectShader.setFloat("shininess", lightCHandle->Shininess);
		objectShader.setFloat("diffuseStrength", lightCHandle->DiffuseStrength);


		int window_width, window_height;
		glfwGetWindowSize(windowCHandle->Window, &window_width, &window_height);

		glm::mat4 projection = glm::perspective(45.0f, (float)window_width / (float)window_height, 0.1f, 100.0f);
		objectShader.setMat4("model", glm::mat4());
		objectShader.setMat4("projection", projection);

		// 渲染，就是之前 Mesh 类的 Draw()
		world->each<ObjectComponent>([&](Entity* ent, ComponentHandle<ObjectComponent> objectCHandle) -> void {
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
				glDepthFunc(GL_LEQUAL);
				glBindVertexArray(mesh.VAO);
				glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				glDepthFunc(GL_LESS);
				glActiveTexture(GL_TEXTURE0);
			}

			
		});

			skyboxShader.use();
			//设置天空盒着色器变量
			glm::mat4 view = glm::mat4(glm::mat3(cameraCHandle->CameraViewMatrix));
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);

			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
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
};