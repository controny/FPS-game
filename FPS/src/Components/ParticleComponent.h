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
	int MAX_TOTAL_NUM; // used to control the exsiting time of particles
	int producedParticles; // not to render particles by default

	int maxParticles;
	int particlesCount;
	int lastUsedParticle;
	float life;
	float spread;
	// number of new particles per millisecond
	int newParticlesPerMS;
	// color settings
	int color_r, color_g, color_b, color_a;
	// main direction of explosion
	glm::vec3 maindir;
	float size;
	bool randomSize;

	GLfloat* g_particule_position_size_data;
	GLubyte* g_particule_color_data;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	GLuint billboard_vertex_buffer;
	GLuint texture;

	Particle* container;

	int id;	// used to identify different entities

	Resource::ParticleResource resource;

	ParticleComponent(Resource::ParticleResource _resource, int _id, int _maxParticles = 500, float _life = 0.3f, int _newParticlesPerMS = 10, float _spread = 1.5f,
		glm::vec3 _maindir = glm::vec3(0.0f, 5.0f, 0.0f), int _color_r = 256, int _color_g = 256, int _color_b = 256, int _color_a = 128)
		: resource(_resource), id(_id), maxParticles(_maxParticles), life(_life), newParticlesPerMS(_newParticlesPerMS), spread(_spread),
		maindir(_maindir), color_r(_color_r), color_g(_color_g), color_b(_color_b), color_a(_color_a)
	{
		MAX_TOTAL_NUM = 10000;
		randomSize = true;
		container = new Particle[maxParticles];
		g_particule_position_size_data = new GLfloat[maxParticles * 4];
		g_particule_color_data = new GLubyte[maxParticles * 4];
		
		reset();

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

	void reset() {
		producedParticles = MAX_TOTAL_NUM;
		lastUsedParticle = 0;

		for (int i = 0; i<maxParticles; i++) {
			container[i].life = -1.0f;
			container[i].cameradistance = -1.0f;
		}
	}

};