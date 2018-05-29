#pragma once
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <ECS.h>
#include <Components/ObjectComponent.h>
#include <Components/TextComponent.h>
#include <Components/CameraInfoSingletonComponent.h>
#include <Components/WindowInfoSingletonComponent.h>
#include <Components/LightingInfoSingletonComponent.h>
#include <Components/SkyboxInfoSingletonComponent.h>
//#include <ogldev_app.cpp>
using namespace ECS;

// 获取所有的 mesh 组件并渲染
class RenderSystem : public EntitySystem {
public:

	Shader objectShader;
	Shader textShader;
	Shader skyboxShader;
	Shader postShader;
	Shader boneShader;

	RenderSystem() {
		objectShader.init("src/Shaders/object.vs", "src/Shaders/object.fs");
		textShader.init("src/Shaders/text.vs", "src/Shaders/text.fs");
		skyboxShader.init("src/Shaders/skybox.vs", "src/Shaders/skybox.fs");
		postShader.init("src/Shaders/post.vs", "src/Shaders/post.fs");
		boneShader.init("src/Shaders/skinning.vs", "src/Shaders/skinning.fs");
	}

	virtual void tick(class World* world, float deltaTime) override
	{
		auto cameraCHandle = world->getSingletonComponent<CameraInfoSingletonComponent>();
		auto windowCHandle = world->getSingletonComponent<WindowInfoSingletonComponent>();
		auto lightCHandle = world->getSingletonComponent<LightingInfoSingletonComponent>();
		auto SkyboxCHandle = world->getSingletonComponent<SkyboxInfoSingletonComponent>();

		/* ----------- render object -----------*/
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

		glm::mat4 projection = glm::perspective(45.0f, (float)window_width / (float)window_height, 0.1f, 1000.0f);
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

		cout << cameraCHandle->CameraPos.x << " " << cameraCHandle->CameraPos.y << " " << cameraCHandle->CameraPos.z << endl;
		//设置骨骼着色器变量
		glm::mat4 bonemodel = glm::scale(glm::mat4(), glm::vec3(0.1f, 0.1f, 0.1f));
		
		boneShader.use();
		boneShader.setMat4("view", cameraCHandle->CameraViewMatrix);
		boneShader.setVec3("viewPos", cameraCHandle->CameraPos);

		boneShader.setVec3("lightPos", lightCHandle->LightPos);
		boneShader.setVec3("lightColor", lightCHandle->LightColor);
		boneShader.setFloat("ambientStrength", lightCHandle->AmbientStrength);
		boneShader.setFloat("specularStrength", lightCHandle->SpecularStrength);
		boneShader.setFloat("shininess", lightCHandle->Shininess);
		boneShader.setFloat("diffuseStrength", lightCHandle->DiffuseStrength);

		bonemodel = glm::rotate(bonemodel, 180.0f, glm::vec3(1.0f, 1.0f, 1.0f));
		boneShader.setMat4("model", bonemodel);
		boneShader.setMat4("projection", projection);
		
		// 渲染骨骼模型
		world->each<BoneObjectComponent>([&](Entity* ent, ComponentHandle<BoneObjectComponent> BoneobjectCHandle) -> void {
			BoneobjectCHandle->m_pScene= BoneobjectCHandle->m_Importer.ReadFile(BoneobjectCHandle->filename, ASSIMP_LOAD_FLAGS);
			static vector<Matrix4f> Transforms;
			static int renderCount = 0;
			//float RunningTime = GetRunningTime();
			static float RunningTime = 0.5;
			RunningTime += 0.1;
			if (renderCount == 0) {
				BoneobjectCHandle->BoneTransform(RunningTime, Transforms);
				renderCount++;
			}
			BoneobjectCHandle->BoneTransform(RunningTime, Transforms);
			renderCount++;
			GLuint m_boneLocation[100];
			for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
				char Name[128];
				memset(Name, 0, sizeof(Name));
				SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);
				m_boneLocation[i] = glGetUniformLocation(boneShader.ID, Name);
			}
			for (uint i = 0; i < Transforms.size(); i++) {
				//m_pEffect->SetBoneTransform(i, Transforms[i]);
				glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat*)Transforms[i]);
			}
			
			
			BoneobjectCHandle->Render();
		});



		
		/* ----------- render skybox -----------*/
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


		/* ----------- render text -----------*/
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		textShader.use();
		projection = glm::ortho(0.0f, (float)window_width, 0.0f, (float)window_height, -0.001f, 1.0f);
		textShader.setMat4("projection", projection);

		world->each<TextComponent>([&](Entity* ent, ComponentHandle<TextComponent> textCHandle) {
			textShader.setVec3("textColor", textCHandle->color);
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(textCHandle->VAO);

			// Iterate through all characters
			float origin_x = textCHandle->x;

			std::string::const_iterator c;
			for (c = textCHandle->text.begin(); c != textCHandle->text.end(); c++)
			{
				Character ch = textCHandle->Characters[*c];

				GLfloat xpos = textCHandle->x + ch.Bearing.x * textCHandle->scale;
				GLfloat ypos = textCHandle->y - (ch.Size.y - ch.Bearing.y) * textCHandle->scale;

				GLfloat w = ch.Size.x * textCHandle->scale;
				GLfloat h = ch.Size.y * textCHandle->scale;
				// Update VBO for each character
				GLfloat vertices[6][4] = {
					{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
				};
				// Render glyph texture over quad
				glBindTexture(GL_TEXTURE_2D, ch.TextureID);
				// Update content of VBO memory
				glBindBuffer(GL_ARRAY_BUFFER, textCHandle->VBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// Render quad
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
				textCHandle->x += (ch.Advance >> 6) * textCHandle->scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
			}
			textCHandle->x = origin_x;
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		});


		/* ----------- render post -----------*/
		postShader.use();
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
};