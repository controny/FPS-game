#pragma once
#include <glad/glad.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shaders/shader.h>
#include <camera/camera.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <ECS.h>
#include "../MeshComponent.h"
#include "../Components/CameraInfoSingletonComponent.h"
#include "../Components/WindowInfoSingletonComponent.h"
#include "../Components/LightingInfoSingletonComponent.h"

using namespace ECS;

class RenderSystem : public EntitySystem {
public:

	Shader objectShader;
	Camera camera;

	RenderSystem() {
		objectShader.init("src/shaderPrograms/object.vs", "src/shaderPrograms/object.fs");
	}

	virtual void tick(class World* world, float deltaTime) override
	{
		objectShader.use();

		world->each<CameraInfoSingletonComponent>([&](Entity* ent, ComponentHandle<CameraInfoSingletonComponent> c) -> void {
			objectShader.setMat4("view", c->CameraViewMatrix);
			objectShader.setVec3("viewPos", c->CameraPos);
		});

		world->each<LightingInfoSingletonComponent>([&](Entity* ent, ComponentHandle<LightingInfoSingletonComponent> c) -> void {
			objectShader.setVec3("lightPos", c->LightPos);
			objectShader.setVec3("lightColor", c->LightColor);
			objectShader.setFloat("ambientStrength", c->AmbientStrength);
			objectShader.setFloat("specularStrength", c->SpecularStrength);
			objectShader.setFloat("shininess", c->Shininess);
			objectShader.setFloat("diffuseStrength", c->DiffuseStrength);
		});

		world->each<WindowInfoSingletonComponent>([&](Entity* ent, ComponentHandle<WindowInfoSingletonComponent> c) -> void {
			glm::mat4 projection = glm::perspective(45.0f, (float)c->Width / (float)c->Height, 0.1f, 100.0f);
			objectShader.setMat4("model", glm::mat4());
			objectShader.setMat4("projection", projection);
		});

		world->each<MeshComponent>([&](Entity* ent, ComponentHandle<MeshComponent> mesh) -> void {
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;
			unsigned int normalNr = 1;
			unsigned int heightNr = 1;
			for (unsigned int i = 0; i < mesh->textures.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
												  // retrieve texture number (the N in diffuse_textureN)
				string number;
				string name = mesh->textures[i].type;
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
				glBindTexture(GL_TEXTURE_2D, mesh->textures[i].id);
			}

			glBindVertexArray(mesh->VAO);
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		});
	}
};