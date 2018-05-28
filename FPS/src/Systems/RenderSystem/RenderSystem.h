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

// 获取所有的 mesh 组件并渲染
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
		auto SkyboxCHandle = world->getSingletonComponent<SkyboxInfoSingletonComponent>();

		glm::mat4 ViewMatrix;
		glm::vec3 CameraPos;
		world->each<PlayerComponent>([&](Entity* ent, ComponentHandle<PlayerComponent> playerCHandle) -> void {
			auto cameraCHandle = ent->get<CameraComponent>();
			auto positionCHandle = ent->get<PositionComponent>();

			glm::vec3 XZ_front = glm::normalize(glm::vec3(positionCHandle->Front.x, 0.0f, positionCHandle->Front.z));
			CameraPos = positionCHandle->Position + glm::vec3(XZ_front.x, cameraCHandle->Relative_position.y, XZ_front.z);

			ViewMatrix = glm::lookAt(CameraPos, CameraPos + positionCHandle->Front, positionCHandle->Up);
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

				//cout << glm::asin(XZ_front.x) << endl;
				//cout << x << ' ' << z << endl;
				//cout << glm::acos(z) << endl;

				//model = glm::rotate(model, glm::asin(XZ_front.x), glm::vec3(0.0, 1.0, 0.0));
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


		/* ----------- render skybox -----------*/
		skyboxShader.use();

		//设置天空盒着色器变量
		glm::mat4 view = glm::mat4(glm::mat3(ViewMatrix));
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
		
		/* ----------- render particles -----------*/
		world->each<ParticleComponent>([&](Entity* ent, ComponentHandle<ParticleComponent> particleCHandle) {
			// Accept fragment if it closer to the camera than the former one
			glDepthFunc(GL_LESS);

			GLuint VertexArrayID;
			glGenVertexArrays(1, &VertexArrayID);
			glBindVertexArray(VertexArrayID);

			// Generate 10 new particule each millisecond,
			// but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
			// newparticles will be huge and the next frame even longer.
			int newparticles = (int)(deltaTime*10000.0);
			if (newparticles > (int)(0.016f*10000.0))
				newparticles = (int)(0.016f*10000.0);

			generateNewParticles(particleCHandle, newparticles);
			int ParticlesCount = simulateAllParticles(particleCHandle, cameraCHandle, deltaTime);

			// Update the buffers that OpenGL uses for rendering.
			// There are much more sophisticated means to stream data from the CPU to the GPU, 
			// but this is outside the scope of this tutorial.
			// http://www.opengl.org/wiki/Buffer_Object_Streaming
			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->particles_position_buffer);
			glBufferData(GL_ARRAY_BUFFER, particleCHandle->maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
			glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, particleCHandle->g_particule_position_size_data);

			glBindBuffer(GL_ARRAY_BUFFER, particleCHandle->particles_color_buffer);
			glBufferData(GL_ARRAY_BUFFER, particleCHandle->maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
			glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, particleCHandle->g_particule_color_data);


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
			glm::mat4 viewMatrix = cameraCHandle->CameraViewMatrix;
			// Projection matrix : 45?Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
			glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
			particleShader.setVec3("CameraRight_worldspace",
				viewMatrix[0][0],
				viewMatrix[1][0],
				viewMatrix[2][0]);
			particleShader.setVec3("CameraUp_worldspace",
				viewMatrix[0][1],
				viewMatrix[1][1],
				viewMatrix[2][1]);
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
										 // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
										 // but faster.
			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		});
	}

private:
	/* ----------- private methods for rendering particles -----------*/
	// Finds a Particle in particleCHandle->container which isn't used yet.
	// (i.e. life < 0);
	int FindUnusedParticle(ComponentHandle<ParticleComponent> particleCHandle) {

		for (int i = particleCHandle->lastUsedParticle; i < particleCHandle->maxParticles; i++) {
			if (particleCHandle->container[i].life < 0) {
				particleCHandle->lastUsedParticle = i;
				return i;
			}
		}

		for (int i = 0; i < particleCHandle->lastUsedParticle; i++) {
			if (particleCHandle->container[i].life < 0) {
				particleCHandle->lastUsedParticle = i;
				return i;
			}
		}

		return 0; // All particles are taken, override the first one
	}

	void SortParticles(ComponentHandle<ParticleComponent> particleCHandle) {
		std::sort(&particleCHandle->container[0], &particleCHandle->container[particleCHandle->maxParticles]);
	}

	void generateNewParticles(ComponentHandle<ParticleComponent> particleCHandle, int newparticles) {
		for (int i = 0; i < newparticles; i++) {
			int particleIndex = FindUnusedParticle(particleCHandle);
			particleCHandle->container[particleIndex].life = particleCHandle->life;
			particleCHandle->container[particleIndex].pos = particleCHandle->position;

			float spread = 1.5f;
			// Very bad way to generate a random direction; 
			// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
			// combined with some user-controlled parameters (main direction, spread, etc)
			glm::vec3 randomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);

			particleCHandle->container[particleIndex].speed = particleCHandle->maindir + randomdir * spread;


			// Very bad way to generate a random color
			particleCHandle->container[particleIndex].r = rand() % particleCHandle->max_r;
			particleCHandle->container[particleIndex].g = rand() % particleCHandle->max_g;
			particleCHandle->container[particleIndex].b = rand() % particleCHandle->max_b;
			particleCHandle->container[particleIndex].a = (rand() % 256) / 3;

			particleCHandle->container[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

		}
	}

	// Simulate all particles
	int simulateAllParticles(
		ComponentHandle<ParticleComponent> particleCHandle,
		ComponentHandle<CameraInfoSingletonComponent> cameraCHandle,
		float deltaTime) {

		int ParticlesCount = 0;
		for (int i = 0; i < particleCHandle->maxParticles; i++) {

			Particle& p = particleCHandle->container[i]; // shortcut

			if (p.life > 0.0f) {

				// Decrease life
				p.life -= deltaTime;
				if (p.life > 0.0f) {

					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)deltaTime * 0.5f;
					p.pos += p.speed * (float)deltaTime;
					p.cameradistance = glm::length2(p.pos - cameraCHandle->CameraPos);
					//particleCHandle->container[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

					// Fill the GPU buffer
					particleCHandle->g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
					particleCHandle->g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
					particleCHandle->g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

					particleCHandle->g_particule_position_size_data[4 * ParticlesCount + 3] = p.size;

					particleCHandle->g_particule_color_data[4 * ParticlesCount + 0] = p.r;
					particleCHandle->g_particule_color_data[4 * ParticlesCount + 1] = p.g;
					particleCHandle->g_particule_color_data[4 * ParticlesCount + 2] = p.b;
					particleCHandle->g_particule_color_data[4 * ParticlesCount + 3] = p.a;

				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameradistance = -1.0f;
				}

				ParticlesCount++;

			}
		}

		SortParticles(particleCHandle);

		//printf("%d ",ParticlesCount);
		return ParticlesCount;
	}
};
