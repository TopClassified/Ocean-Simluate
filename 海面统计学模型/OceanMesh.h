#pragma once
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

#include<GL\Shader.h>
#include"Complex.h"
#include"FFT.h"

#define This_PI 3.1415926

struct vertex_ocean 
{
	GLfloat   x, y, z;   // 海面模型的当前顶点坐标
	GLfloat  nx, ny, nz; // 海面模型的顶点法线向量
	GLfloat   a, b, c;   // 波浪高度函数的三个变量（？）
	GLfloat  _a, _b, _c; // htilde0mk conjugate
	GLfloat  ox, oy, oz; // 海面模型的原始顶点坐标
};

struct Map_param 
{
	GLfloat a, b, _a, _b;
};

class OceanMesh
{
public:
	OceanMesh(const int N, const float A, const glm::vec2 w, const float length);
	~OceanMesh();

	void release();
	float dispersion(int n_prime, int m_prime);				// 计算W(k)的值（离散化）
	float phillips(int n_prime, int m_prime);				// 菲利普斯频谱
	Complex hTilde_0(int n_prime, int m_prime);				// 计算定点的初始高度
	Complex hTilde(float t, int n_prime, int m_prime);		// 计算当前顶点的高度
	void evaluateWavesFFT(float t);
	void render(float t, glm::vec3 light_pos, glm::mat4 Projection, glm::mat4 View, glm::mat4 Model, Shader RenderShader);
private:
	bool geometry;							// 区分渲染结合体或者表面的标记

	float g;								// 重力常数
	int N, Nplus1;							// 维度（即方程有多少项） -- N应该是2的倍数
	float A;								// 菲利普频谱的参数 -- 影响波的高度（振幅）
	glm::vec2 w;							// 风的参数
	float length;							// 长度参数
	Complex *h_tilde;						// 存储经过傅里叶变换后计算出来的波高
	Complex	*h_tilde_slopex;				// 
	Complex *h_tilde_slopez;
	Complex	*h_tilde_dx;
	Complex *h_tilde_dz;
	FFT *fft;								// 快速傅里叶变换成员类

	vertex_ocean *vertices;					// 海面模型的顶点数组
	unsigned int *indices;					// 顶点数组的索引
	unsigned int indices_count;				// 需要绘制的索引的数量
	GLuint VAO, VBO, EBO;	
};

