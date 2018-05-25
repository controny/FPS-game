#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <Resource.h>

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if <0 : dead and unused.
	float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

struct ParticleComponent {
	int maxParticles;
	int lastUsedParticle = 0;
	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	GLuint billboard_vertex_buffer;
	GLuint texture;

	Particle* container;

	ParticleComponent(int _maxParticles)
	{
		maxParticles = _maxParticles;
		container = new Particle[maxParticles];
		g_particule_position_size_data = new GLfloat[maxParticles * 4];
		g_particule_color_data = new GLubyte[maxParticles * 4];
		
		texture = loadDDS("resources/textures/particle.DDS");

		for (int i = 0; i<maxParticles; i++) {
			container[i].life = -1.0f;
			container[i].cameradistance = -1.0f;
		}

		// The VBO containing the 4 vertices of the particles.
		// Thanks to instancing, they will be shared by all particles.
		const GLfloat g_vertex_buffer_data[] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f,
			0.5f,  0.5f, 0.0f,
		};
		glGenBuffers(1, &billboard_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		// The VBO containing the positions and sizes of the particles
		glGenBuffers(1, &particles_position_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

		// The VBO containing the colors of the particles
		glGenBuffers(1, &particles_color_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
		// Initialize with empty (NULL) buffer : it will be updated later, each frame.
		glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
	}
};