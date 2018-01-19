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
	GLfloat   x, y, z;   // ����ģ�͵ĵ�ǰ��������
	GLfloat  nx, ny, nz; // ����ģ�͵Ķ��㷨������
	GLfloat   a, b, c;   // ���˸߶Ⱥ�������������������
	GLfloat  _a, _b, _c; // htilde0mk conjugate
	GLfloat  ox, oy, oz; // ����ģ�͵�ԭʼ��������
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
	float dispersion(int n_prime, int m_prime);				// ����W(k)��ֵ����ɢ����
	float phillips(int n_prime, int m_prime);				// ������˹Ƶ��
	Complex hTilde_0(int n_prime, int m_prime);				// ���㶨��ĳ�ʼ�߶�
	Complex hTilde(float t, int n_prime, int m_prime);		// ���㵱ǰ����ĸ߶�
	void evaluateWavesFFT(float t);
	void render(float t, glm::vec3 light_pos, glm::mat4 Projection, glm::mat4 View, glm::mat4 Model, Shader RenderShader);
private:
	bool geometry;							// ������Ⱦ�������߱���ı��

	float g;								// ��������
	int N, Nplus1;							// ά�ȣ��������ж���� -- NӦ����2�ı���
	float A;								// ������Ƶ�׵Ĳ��� -- Ӱ�첨�ĸ߶ȣ������
	glm::vec2 w;							// ��Ĳ���
	float length;							// ���Ȳ���
	Complex *h_tilde;						// �洢��������Ҷ�任���������Ĳ���
	Complex	*h_tilde_slopex;				// 
	Complex *h_tilde_slopez;
	Complex	*h_tilde_dx;
	Complex *h_tilde_dz;
	FFT *fft;								// ���ٸ���Ҷ�任��Ա��

	vertex_ocean *vertices;					// ����ģ�͵Ķ�������
	unsigned int *indices;					// �������������
	unsigned int indices_count;				// ��Ҫ���Ƶ�����������
	GLuint VAO, VBO, EBO;	
};

