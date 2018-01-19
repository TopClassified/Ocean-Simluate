#pragma once

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <GL/Shader.h>

// Number of vertices on one side of the ocean grid. Has to be power of two.
#define N 512

// Length of the ocean grid.
#define LENGTH	250.0f

// Gravity.
#define GRAVITY 9.81f

// Amplitude of the wave.
#define AMPLITUDE 0.002f

// Wind speed.
#define WIND_SPEED 10.0f

// Wind direction.
#define WIND_DIRECTION {1.0f, 1.0f}

// Largest possible wave arising.
#define LPWA (WIND_SPEED * WIND_SPEED / GRAVITY)

#define GLUS_UNIFORM_RANDOM_BIAS 0.0000001f

class OceanMeshForGPU
{
public:
	OceanMeshForGPU();
	~OceanMeshForGPU();
	GLfloat PhillipsSpectrum(GLfloat A, GLfloat L, glm::vec2 waveDirection, glm::vec2 windDirection);
	GLboolean Init();
	GLboolean update(GLfloat time, glm::mat4 Model, glm::mat4 View, glm::mat4 Perspective, glm::vec3 CamPos, GLuint SkyText);
	glm::mat4 GetReflectMatrix()
	{
		return this->ReflectMatrix;
	}
private:
	GLint g_totalTimeUpdateHtLocation;
	GLint g_processColumnFftLocation;
	GLint g_stepsFftLocation;
	GLint g_ModelLoc;
	GLint g_ViewLoc;
	GLint g_ProjectionLoc;
	GLint g_normalMatrixLocation;
	GLint g_lightDirectionLocation;
	GLint g_colorLocation;
	GLint g_vertexLocation;
	GLint g_texCoordLocation;
	GLuint g_numberIndices;
	GLuint g_verticesVBO;
	GLuint g_texCoordsVBO;
	GLuint g_indicesVBO;
	GLuint g_textureH0;
	GLuint g_textureHt;
	GLuint g_textureIndices;
	GLuint g_textureDisplacement[2];
	GLuint g_textureNormal;
	GLuint g_vao;

	Shader *g_computeUpdateHtProgram;
	Shader *g_computeFftProgram;
	Shader *g_computeUpdateNormalProgram;
	Shader *g_program;

	glm::mat4 ReflectMatrix;
	glm::vec4 OceanPlane = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
};

