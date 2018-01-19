
#include "OceanMeshForGPU.h"

#include "GLShape.h"

#include "GPUFFT.h"


OceanMeshForGPU::OceanMeshForGPU()
{
	//���㷴�����
	ReflectMatrix[0][0] = (1.0f - 2.0f * OceanPlane[0] * OceanPlane[0]);
	ReflectMatrix[0][1] = (-2.0f * OceanPlane[0] * OceanPlane[1]);
	ReflectMatrix[0][2] = (-2.0f * OceanPlane[0] * OceanPlane[2]);
	ReflectMatrix[0][3] = (-2.0f * OceanPlane[3] * OceanPlane[0]);

	ReflectMatrix[1][0] = (-2.0f * OceanPlane[1] * OceanPlane[0]);
	ReflectMatrix[1][1] = (1.0f - 2.0f * OceanPlane[1] * OceanPlane[1]);
	ReflectMatrix[1][2] = (-2.0f * OceanPlane[1] * OceanPlane[2]);
	ReflectMatrix[1][3] = (-2.0f * OceanPlane[3] * OceanPlane[1]);

	ReflectMatrix[2][0] = (-2.0f * OceanPlane[2] * OceanPlane[0]);
	ReflectMatrix[2][1] = (-2.0f * OceanPlane[2] * OceanPlane[1]);
	ReflectMatrix[2][2] = (1.0f - 2.0f * OceanPlane[2] * OceanPlane[2]);
	ReflectMatrix[2][3] = (-2.0f * OceanPlane[3] * OceanPlane[2]);

	ReflectMatrix[3][0] = 0.0f;
	ReflectMatrix[3][1] = 0.0f;
	ReflectMatrix[3][2] = 0.0f;
	ReflectMatrix[3][3] = 1.0f;
}


OceanMeshForGPU::~OceanMeshForGPU()
{
}

GLfloat  RandomUniform(const GLfloat start, const GLfloat end)
{
	return ((GLfloat)rand() / (GLfloat)RAND_MAX) * (end - start) + start;
}

GLfloat  RandomNormal(const GLfloat mean, const GLfloat standardDeviation)
{
	GLfloat x1, x2;

	x1 = RandomUniform(GLUS_UNIFORM_RANDOM_BIAS, 1.0f - GLUS_UNIFORM_RANDOM_BIAS);
	x2 = RandomUniform(0.0f, 1.0f);

	return mean + standardDeviation * (sqrtf(-2.0f * logf(x1)) * cosf(2.0f * GL_PI * x2));
}

GLfloat OceanMeshForGPU::PhillipsSpectrum(GLfloat A, GLfloat L, glm::vec2 waveDirection, glm::vec2 windDirection)
{
	GLfloat k = glm::length(waveDirection);
	GLfloat waveDotWind = glm::dot(waveDirection, windDirection);

	if (L == 0.0f || k == 0.0f)
	{
		return 0.0f;
	}

	return A * expf(-1.0f / (k * L * k * L)) / (k * k * k * k) * waveDotWind * waveDotWind;
}

GLboolean OceanMeshForGPU::Init()
{
	glm::vec3 lightDirection = { 0.5f, 1.0f, 1.0f };
	glm::vec4 color = { 0.0f, 0.8f, 0.8f, 1.0f };

	GLUSshape gridPlane;

	GLint i, k;
	glm::mat4 matrix = glm::mat4();

	GLfloat* h0Data;

	GLint* butterflyIndices;
	GLfloat* butterflyIndicesAsFloat;

	glm::vec2 waveDirection;
	glm::vec2 windDirection = WIND_DIRECTION;
	GLfloat phillipsSpectrumValue;

	//ͨ��������������� FFT ��ʱ����Ҫ�ĵ����ܴ���
	GLint steps = 0;
	GLint temp = N;
	while (!(temp & 0x1))
	{
		temp = temp >> 1;
		steps++;
	}
	//�������º�������ļ�����ɫ��
	g_computeUpdateHtProgram = new Shader("OceanUpdate.comp");
	//��������FFT�ļ�����ɫ��
	g_computeFftProgram = new Shader("OceanFFT.comp");
	//�������·��������ļ�����ɫ��
	g_computeUpdateNormalProgram = new Shader("OceanUpdateNormal.comp");
	//����������Ⱦ�Ķ����������ɫ��
	g_program = new Shader("OceanGPU.vs", "OceanGPU.frag");
	
	g_totalTimeUpdateHtLocation = glGetUniformLocation(g_computeUpdateHtProgram->Program , "u_totalTime");

	g_processColumnFftLocation = glGetUniformLocation(g_computeFftProgram->Program, "u_processColumn");
	g_stepsFftLocation = glGetUniformLocation(g_computeFftProgram->Program, "u_steps");

	g_ModelLoc = glGetUniformLocation(g_program->Program, "u_Model");
	g_ViewLoc = glGetUniformLocation(g_program->Program, "u_View");
	g_ProjectionLoc = glGetUniformLocation(g_program->Program, "u_Projection");
	g_normalMatrixLocation = glGetUniformLocation(g_program->Program, "u_normalMatrix");
	g_lightDirectionLocation = glGetUniformLocation(g_program->Program, "u_lightDirection");
	g_colorLocation = glGetUniformLocation(g_program->Program, "u_color");

	g_vertexLocation = glGetAttribLocation(g_program->Program, "a_vertex");
	g_texCoordLocation = glGetAttribLocation(g_program->Program, "a_texCoord");

	// ����һ�����ε�����ƽ��
	CreateRectangularGridPlane(&gridPlane, LENGTH, LENGTH, N - 1, N - 1, GL_FALSE);
	//��ȡ���������������
	g_numberIndices = gridPlane.numberIndices;

	// ����ǰ������X��Ϊ������ת90��ʹ�䴦��X-Zƽ��
	matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (i = 0; i < gridPlane.numberVertices; i++)
	{
		gridPlane.vertices[i] = matrix * gridPlane.vertices[i];
	}

	//�洢���������VBO
	glGenBuffers(1, &g_verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, gridPlane.numberVertices * 4 * sizeof(GLfloat), (GLfloat*)gridPlane.vertices, GL_STATIC_DRAW);
	//�洢�������������VBO
	glGenBuffers(1, &g_texCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_texCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, gridPlane.numberVertices * 2 * sizeof(GLfloat), (GLfloat*)gridPlane.texCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//�洢������VBO
	glGenBuffers(1, &g_indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridPlane.numberIndices * sizeof(GLuint), (GLuint*)gridPlane.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ShapeDestroy(&gridPlane);

	//h0Data�洢����ģ�͵ĳ�ʼֵ
	h0Data = (GLfloat*)malloc(N * N * 2 * sizeof(GLfloat));
	if (!h0Data)
	{
		return GL_FALSE;
	}
	//PhillipsƵ�׼������ʼ����
	windDirection = glm::normalize(windDirection);
	for (i = 0; i < N; i++)
	{
		waveDirection[1] = ((GLfloat)i - (GLfloat)N / 2.0f) * (2.0f * GL_PI / LENGTH);
		for (k = 0; k < N; k++)
		{
			waveDirection[0] = ((GLfloat)k - (GLfloat)N / 2.0f) * (2.0f * GL_PI / LENGTH);
			phillipsSpectrumValue = PhillipsSpectrum(AMPLITUDE, LPWA, waveDirection, windDirection);
			h0Data[i * 2 * N + k * 2 + 0] = 1.0f / sqrtf(2.0f) * RandomNormal(0.0f, 1.0f) * phillipsSpectrumValue;
			h0Data[i * 2 * N + k * 2 + 1] = 1.0f / sqrtf(2.0f) * RandomNormal(0.0f, 1.0f) * phillipsSpectrumValue;
		}
	}

	//�����ݴ洢��һ����ͼ���У�g_textureH0��ͼ��R��G����ͨ���ֱ�洢h0Data���൱�ڸ�������ʵ�����鲿
	glGenTextures(1, &g_textureH0);
	glBindTexture(GL_TEXTURE_2D, g_textureH0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, h0Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	free(h0Data);

	//���ڴ洢���˺�����������ĸ���ֵ����ͼ
	glGenTextures(1, &g_textureHt);
	glBindTexture(GL_TEXTURE_2D, g_textureHt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//���ڴ洢����ƫ��ֵ����ͼ�����м���õ���ֵ��
	glGenTextures(1, &g_textureDisplacement[0]);
	glBindTexture(GL_TEXTURE_2D, g_textureDisplacement[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//���ڴ洢����ƫ��ֵ����ͼ�����м���õ���ֵ��
	glGenTextures(1, &g_textureDisplacement[1]);
	glBindTexture(GL_TEXTURE_2D, g_textureDisplacement[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//�洢��������ֵ
	glGenTextures(1, &g_textureNormal);
	glBindTexture(GL_TEXTURE_2D, g_textureNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	//�����渵��Ҷ�任����Ҫ�ĵ����㷨ʹ�õ�����ֵ
	butterflyIndices = (GLint*)malloc(N * sizeof(GLint));
	if (!butterflyIndices)
	{
		return GL_FALSE;
	}
	//ʹ��GPU����FFT��Ҫ�Ȱѵ�һ�ε������������������N=8ʱ ����Ϊ(0,4) (1,5) (2,6) (3,7)
	butterflyIndicesAsFloat = (GLfloat*)malloc(N * sizeof(GLfloat));
	if (!butterflyIndicesAsFloat)
	{
		free(butterflyIndices);
		return GL_FALSE;
	}
	for (i = 0; i < N; i++)
	{
		butterflyIndices[i] = i;
	}
	FourierButterflyShuffleFFTi(butterflyIndices, butterflyIndices, N);
	for (i = 0; i < N; i++)
	{
		butterflyIndicesAsFloat[i] = (GLfloat)butterflyIndices[i];
	}
	free(butterflyIndices);

	//������õ�����ֵ�洢����ͼ����
	glGenTextures(1, &g_textureIndices);
	glBindTexture(GL_TEXTURE_1D, g_textureIndices);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, N, 0, GL_RED, GL_FLOAT, butterflyIndicesAsFloat);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_1D, 0);
	free(butterflyIndicesAsFloat);

	//������Ҫ�ĵ����������ݵ�FFT�ļ�����ɫ����
	glUseProgram(g_computeFftProgram->Program);
	glUniform1i(g_stepsFftLocation, steps);

	//������Ⱦʹ�õ�VAO
	glUseProgram(g_program->Program);
	glGenVertexArrays(1, &g_vao);
	glBindVertexArray(g_vao);
	glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
	glVertexAttribPointer(g_vertexLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(g_vertexLocation);
	glBindBuffer(GL_ARRAY_BUFFER, g_texCoordsVBO);
	glVertexAttribPointer(g_texCoordLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(g_texCoordLocation);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_indicesVBO);
	glBindVertexArray(0);

	lightDirection = glm::normalize(lightDirection);
	GLfloat HelpLight[3];
	HelpLight[0] = lightDirection.x;
	HelpLight[1] = lightDirection.y;
	HelpLight[2] = lightDirection.z;
	glUniform3fv(g_lightDirectionLocation, 1, HelpLight);
	GLfloat HelpColor[4];
	HelpColor[0] = color.x;
	HelpColor[1] = color.y;
	HelpColor[2] = color.z;
	HelpColor[3] = color.w;
	glUniform4fv(g_colorLocation, 1, HelpColor);
	glUseProgram(0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return GL_TRUE;
}

GLboolean OceanMeshForGPU::update(GLfloat time, glm::mat4 Model, glm::mat4 View, glm::mat4 Perspective, glm::vec3 CamPos, GLuint ReflectText)
{
	static GLfloat totalTime = 0.0f;

	//������Ҫ������任�����뵽�����õ���ɫ������
	glUseProgram(g_program->Program);
	glm::mat3 normalMatrix = glm::mat3(Model);
	glUniformMatrix4fv(g_ModelLoc, 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(g_ViewLoc, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(g_ProjectionLoc, 1, GL_FALSE, glm::value_ptr(Perspective));
	glUniformMatrix3fv(g_normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(glGetUniformLocation(g_program->Program, "camPos"), 1, &CamPos[0]);
	glUseProgram(0);

	//ʹ��Update������ɫ�����󶨳�ʼֵ��ͼ�ͼ�ʱֵ��ͼ
	glUseProgram(g_computeUpdateHtProgram->Program);
	glBindImageTexture(0, g_textureH0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureHt, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	//��������ʱ��
	glUniform1f(g_totalTimeUpdateHtLocation, totalTime);
	//����һ��N*N��С�Ĺ����飬��ͬʱ�������еĶ���߶�ֵ
	glDispatchCompute(N, N, 1);
	//ȷ�����е����ݶ�д�뵽��ͼ����
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//ʹ��FFT������ɫ���Խ��������任
	glUseProgram(g_computeFftProgram->Program);
	//��������ͼ���ϸ�������ɫ�����ý���Ĳ��˺�����ͼ����Ҫ�洢ƫ��ֵ����ͼ
	glBindImageTexture(0, g_textureHt, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureDisplacement[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(2, g_textureIndices, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
	glUniform1i(g_processColumnFftLocation, 0);
	// �ȶ�ÿһ�н�����任
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, g_textureDisplacement[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureDisplacement[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glUniform1i(g_processColumnFftLocation, 1);
	// �ٶ�ÿһ�н�����任
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//���·�������
	glUseProgram(g_computeUpdateNormalProgram->Program);
	glBindImageTexture(0, g_textureDisplacement[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureNormal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(N, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(g_program->Program);
	glBindVertexArray(g_vao);
	//�����˸߶���ͼ����GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_textureDisplacement[1]);
	//��������ͼ����GL_TEXTURE1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_textureNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ReflectText);
	//����������ʼ����
	glDrawElements(GL_TRIANGLES, g_numberIndices, GL_UNSIGNED_INT, 0);
	//����GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//����GL_TEXTURE1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	totalTime += time;
	return GL_TRUE;
}