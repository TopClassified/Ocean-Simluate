
#include "OceanMeshForGPU.h"

#include "GLShape.h"

#include "GPUFFT.h"


OceanMeshForGPU::OceanMeshForGPU()
{
	//计算反射矩阵
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

	//通过项数计算出计算 FFT 的时候需要的迭代总次数
	GLint steps = 0;
	GLint temp = N;
	while (!(temp & 0x1))
	{
		temp = temp >> 1;
		steps++;
	}
	//声明更新海面网格的计算着色器
	g_computeUpdateHtProgram = new Shader("OceanUpdate.comp");
	//声明进行FFT的计算着色器
	g_computeFftProgram = new Shader("OceanFFT.comp");
	//声明更新法线向量的计算着色器
	g_computeUpdateNormalProgram = new Shader("OceanUpdateNormal.comp");
	//声明进行渲染的顶点和像素着色器
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

	// 创建一个方形的网格平面
	CreateRectangularGridPlane(&gridPlane, LENGTH, LENGTH, N - 1, N - 1, GL_FALSE);
	//获取该网格的索引总数
	g_numberIndices = gridPlane.numberIndices;

	// 将当前网格以X轴为基础旋转90度使其处于X-Z平面
	matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (i = 0; i < gridPlane.numberVertices; i++)
	{
		gridPlane.vertices[i] = matrix * gridPlane.vertices[i];
	}

	//存储顶点坐标的VBO
	glGenBuffers(1, &g_verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, gridPlane.numberVertices * 4 * sizeof(GLfloat), (GLfloat*)gridPlane.vertices, GL_STATIC_DRAW);
	//存储顶点纹理坐标的VBO
	glGenBuffers(1, &g_texCoordsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, g_texCoordsVBO);
	glBufferData(GL_ARRAY_BUFFER, gridPlane.numberVertices * 2 * sizeof(GLfloat), (GLfloat*)gridPlane.texCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//存储索引的VBO
	glGenBuffers(1, &g_indicesVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_indicesVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridPlane.numberIndices * sizeof(GLuint), (GLuint*)gridPlane.indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ShapeDestroy(&gridPlane);

	//h0Data存储波浪模型的初始值
	h0Data = (GLfloat*)malloc(N * N * 2 * sizeof(GLfloat));
	if (!h0Data)
	{
		return GL_FALSE;
	}
	//Phillips频谱计算出初始波形
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

	//将数据存储到一张贴图当中，g_textureH0贴图的R、G两个通道分别存储h0Data（相当于复数）的实部和虚部
	glGenTextures(1, &g_textureH0);
	glBindTexture(GL_TEXTURE_2D, g_textureH0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, h0Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	free(h0Data);

	//用于存储波浪函数计算出来的复数值的贴图
	glGenTextures(1, &g_textureHt);
	glBindTexture(GL_TEXTURE_2D, g_textureHt);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//用于存储顶点偏移值的贴图（按行计算得到的值）
	glGenTextures(1, &g_textureDisplacement[0]);
	glBindTexture(GL_TEXTURE_2D, g_textureDisplacement[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//用于存储顶点偏移值的贴图（按列计算得到的值）
	glGenTextures(1, &g_textureDisplacement[1]);
	glBindTexture(GL_TEXTURE_2D, g_textureDisplacement[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, N, N, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//存储法线向量值
	glGenTextures(1, &g_textureNormal);
	glBindTexture(GL_TEXTURE_2D, g_textureNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, N, N, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	//生成逆傅里叶变换所需要的蝶形算法使用的索引值
	butterflyIndices = (GLint*)malloc(N * sizeof(GLint));
	if (!butterflyIndices)
	{
		return GL_FALSE;
	}
	//使用GPU计算FFT需要先把第一次迭代的索引计算好例：N=8时 索引为(0,4) (1,5) (2,6) (3,7)
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

	//将计算好的索引值存储到贴图当中
	glGenTextures(1, &g_textureIndices);
	glBindTexture(GL_TEXTURE_1D, g_textureIndices);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, N, 0, GL_RED, GL_FLOAT, butterflyIndicesAsFloat);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_1D, 0);
	free(butterflyIndicesAsFloat);

	//将所需要的迭代次数传递到FFT的计算着色器中
	glUseProgram(g_computeFftProgram->Program);
	glUniform1i(g_stepsFftLocation, steps);

	//设置渲染使用的VAO
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

	//将所需要的三大变换矩阵传入到绘制用的着色器当中
	glUseProgram(g_program->Program);
	glm::mat3 normalMatrix = glm::mat3(Model);
	glUniformMatrix4fv(g_ModelLoc, 1, GL_FALSE, glm::value_ptr(Model));
	glUniformMatrix4fv(g_ViewLoc, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(g_ProjectionLoc, 1, GL_FALSE, glm::value_ptr(Perspective));
	glUniformMatrix3fv(g_normalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(glGetUniformLocation(g_program->Program, "camPos"), 1, &CamPos[0]);
	glUseProgram(0);

	//使用Update计算着色器并绑定初始值贴图和即时值贴图
	glUseProgram(g_computeUpdateHtProgram->Program);
	glBindImageTexture(0, g_textureH0, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureHt, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	//传递运行时间
	glUniform1f(g_totalTimeUpdateHtLocation, totalTime);
	//创建一个N*N大小的工作组，即同时计算所有的顶点高度值
	glDispatchCompute(N, N, 1);
	//确保所有的数据都写入到贴图里了
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//使用FFT计算着色器对结果进行逆变换
	glUseProgram(g_computeFftProgram->Program);
	//绑定索引贴图、上个计算着色器所得结果的波浪函数贴图、将要存储偏移值的贴图
	glBindImageTexture(0, g_textureHt, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureDisplacement[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glBindImageTexture(2, g_textureIndices, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
	glUniform1i(g_processColumnFftLocation, 0);
	// 先对每一行进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindImageTexture(0, g_textureDisplacement[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureDisplacement[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32F);
	glUniform1i(g_processColumnFftLocation, 1);
	// 再对每一列进行逆变换
	glDispatchCompute(1, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	//更新法线向量
	glUseProgram(g_computeUpdateNormalProgram->Program);
	glBindImageTexture(0, g_textureDisplacement[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32F);
	glBindImageTexture(1, g_textureNormal, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(N, N, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glUseProgram(g_program->Program);
	glBindVertexArray(g_vao);
	//将波浪高度贴图绑定在GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_textureDisplacement[1]);
	//将法线贴图绑定在GL_TEXTURE1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_textureNormal);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ReflectText);
	//根据索引开始绘制
	glDrawElements(GL_TRIANGLES, g_numberIndices, GL_UNSIGNED_INT, 0);
	//重置GL_TEXTURE0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//重置GL_TEXTURE1
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);

	totalTime += time;
	return GL_TRUE;
}