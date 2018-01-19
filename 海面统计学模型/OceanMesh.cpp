#include "OceanMesh.h"

float uniformRandomVariable() 
{
	return (float)rand() / RAND_MAX;
}

Complex gaussianRandomVariable() 
{
	float x1, x2, w;
	do {
		x1 = 2.f * uniformRandomVariable() - 1.f;
		x2 = 2.f * uniformRandomVariable() - 1.f;
		w = x1 * x1 + x2 * x2;
	} while (w >= 1.f);
	w = sqrt((-2.f * log(w)) / w);
	return Complex(x1 * w, x2 * w);
}

OceanMesh::OceanMesh(const int N, const float A, const glm::vec2 w, const float length) :
	g(9.81), geometry(geometry), N(N), Nplus1(N + 1), A(A), w(w), length(length), vertices(0), indices(0), 
	h_tilde(0), h_tilde_slopex(0), h_tilde_slopez(0), h_tilde_dx(0), h_tilde_dz(0), fft(0)
{
	h_tilde = new Complex[N*N];
	h_tilde_slopex = new Complex[N*N];
	h_tilde_slopez = new Complex[N*N];
	h_tilde_dx = new Complex[N*N];
	h_tilde_dz = new Complex[N*N];
	fft = new FFT(N);
	vertices = new vertex_ocean[Nplus1*Nplus1];
	indices = new unsigned int[Nplus1*Nplus1 * 10];

	int index;

	//CPU计算波浪谱的初始化操作
	Complex htilde0, htilde0mk_conj;
	for (int m_prime = 0; m_prime < Nplus1; m_prime++) 
	{
		for (int n_prime = 0; n_prime < Nplus1; n_prime++) 
		{
			index = m_prime * Nplus1 + n_prime;

			//根据频谱计算出来每个顶点的初始位值
			htilde0 = hTilde_0(n_prime, m_prime);
			htilde0mk_conj = hTilde_0(-n_prime, -m_prime).GetConjugate();
			
			vertices[index].a = htilde0.a;
			vertices[index].b = htilde0.b;
			vertices[index]._a = htilde0mk_conj.a;
			vertices[index]._b = htilde0mk_conj.b;
			//计算出来每个顶点的原始坐标（即海平面没有任何起伏时候的位置）
			vertices[index].ox = vertices[index].x = (n_prime - N / 2.0f) * length / N;
			vertices[index].oy = vertices[index].y = 0.0f;
			vertices[index].oz = vertices[index].z = (m_prime - N / 2.0f) * length / N;
			//初始化原始法线（即海平面没有任何起伏时候的法线向量）
			vertices[index].nx = 0.0f;
			vertices[index].ny = 1.0f;
			vertices[index].nz = 0.0f;
		}
	}

	indices_count = 0;
	for (int m_prime = 0; m_prime < N; m_prime++) 
	{
		for (int n_prime = 0; n_prime < N; n_prime++) 
		{
			index = m_prime * Nplus1 + n_prime;

			//计算每个顶点的索引
			indices[indices_count++] = index;				
			indices[indices_count++] = index + Nplus1;
			indices[indices_count++] = index + Nplus1 + 1;
			indices[indices_count++] = index;
			indices[indices_count++] = index + Nplus1 + 1;
			indices[indices_count++] = index + 1;
		}
	}
}


OceanMesh::~OceanMesh()
{
	if (h_tilde)		
		delete[] h_tilde;
	if (h_tilde_slopex)	
		delete[] h_tilde_slopex;
	if (h_tilde_slopez)	
		delete[] h_tilde_slopez;
	if (h_tilde_dx)		
		delete[] h_tilde_dx;
	if (h_tilde_dz)		
		delete[] h_tilde_dz;
	if (fft)		
		delete fft;
	if (vertices)		
		delete[] vertices;
	if (indices)		
		delete[] indices;
}

void OceanMesh::release() 
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

float OceanMesh::dispersion(int n_prime, int m_prime)
{
	float w_0 = 2.0f * This_PI / 200.0f;
	float kx = This_PI * (2 * n_prime - N) / length;
	float kz = This_PI * (2 * m_prime - N) / length;
	return floor(sqrt(g * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

float OceanMesh::phillips(int n_prime, int m_prime)
{
	glm::vec2 k(This_PI * (2 * n_prime - N) / length, This_PI * (2 * m_prime - N) / length);
	float k_length = glm::length(k);
	if (k_length < 0.000001) 
		return 0.0;

	float k_length2 = k_length  * k_length;
	float k_length4 = k_length2 * k_length2;

	float k_dot_w = glm::dot(glm::normalize(k), glm::normalize(w));
	float k_dot_w2 = k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w * k_dot_w;

	float w_length = glm::length(w);
	float L = w_length * w_length / g;
	float L2 = L * L;

	float damping = 0.001;
	float l2 = L2 * damping * damping;

	return A * exp(-1.0f / (k_length2 * L2)) / k_length4 * k_dot_w2 * exp(-k_length2 * l2);
}

Complex OceanMesh::hTilde_0(int n_prime, int m_prime)
{
	Complex r = gaussianRandomVariable();
	return r * sqrt(phillips(n_prime, m_prime) / 2.0f);
}

Complex OceanMesh::hTilde(float t, int n_prime, int m_prime) 
{
	int index = m_prime * Nplus1 + n_prime;

	Complex htilde0(vertices[index].a, vertices[index].b);
	Complex htilde0mkconj(vertices[index]._a, vertices[index]._b);

	float omegat = dispersion(n_prime, m_prime) * t;

	float cos_ = cos(omegat);
	float sin_ = sin(omegat);

	Complex c0(cos_, sin_);
	Complex c1(cos_, -sin_);

	Complex res = htilde0 * c0 + htilde0mkconj * c1;

	return res;
}

void OceanMesh::evaluateWavesFFT(float t)
{
	float kx, kz, len, lambda = -1.0f;
	int index, index1;

	for (int m_prime = 0; m_prime < N; m_prime++)
	{
		kz = This_PI * (2.0f * m_prime - N) / length;
		for (int n_prime = 0; n_prime < N; n_prime++)
		{
			kx = This_PI*(2 * n_prime - N) / length;
			len = sqrt(kx * kx + kz * kz);
			index = m_prime * N + n_prime;

			h_tilde[index] = hTilde(t, n_prime, m_prime);
			h_tilde_slopex[index] = h_tilde[index] * Complex(0, kx);
			h_tilde_slopez[index] = h_tilde[index] * Complex(0, kz);
			if (len < 0.000001f)
			{
				h_tilde_dx[index] = Complex(0.0f, 0.0f);
				h_tilde_dz[index] = Complex(0.0f, 0.0f);
			}
			else
			{
				h_tilde_dx[index] = h_tilde[index] * Complex(0, -kx / len);
				h_tilde_dz[index] = h_tilde[index] * Complex(0, -kz / len);
			}
		}
	}
	//按照“行”进行傅里叶变换
	for (int m_prime = 0; m_prime < N; m_prime++)
	{
		fft->CalculateFFT(h_tilde, h_tilde, 1, m_prime * N);
		fft->CalculateFFT(h_tilde_slopex, h_tilde_slopex, 1, m_prime * N);
		fft->CalculateFFT(h_tilde_slopez, h_tilde_slopez, 1, m_prime * N);
		fft->CalculateFFT(h_tilde_dx, h_tilde_dx, 1, m_prime * N);
		fft->CalculateFFT(h_tilde_dz, h_tilde_dz, 1, m_prime * N);
	}
	//按照“列”进行傅里叶变换
	for (int n_prime = 0; n_prime < N; n_prime++)
	{
		fft->CalculateFFT(h_tilde, h_tilde, N, n_prime);
		fft->CalculateFFT(h_tilde_slopex, h_tilde_slopex, N, n_prime);
		fft->CalculateFFT(h_tilde_slopez, h_tilde_slopez, N, n_prime);
		fft->CalculateFFT(h_tilde_dx, h_tilde_dx, N, n_prime);
		fft->CalculateFFT(h_tilde_dz, h_tilde_dz, N, n_prime);
	}

	int sign;
	float signs[] = { 1.0f, -1.0f };
	glm::vec3 n;
	for (int m_prime = 0; m_prime < N; m_prime++)
	{
		for (int n_prime = 0; n_prime < N; n_prime++)
		{
			// 获得该顶点在波浪函数中的索引
			index = m_prime * N + n_prime;			
			//获得该顶点在顶点数组中的索引位置
			index1 = m_prime * Nplus1 + n_prime;	
			sign = signs[(n_prime + m_prime) & 1];

			h_tilde[index] = h_tilde[index] * sign;

			// 获得该顶点的最终高度值
			vertices[index1].y = h_tilde[index].a;

			// 获得改顶点的平面扰动值
			h_tilde_dx[index] = h_tilde_dx[index] * sign;
			h_tilde_dz[index] = h_tilde_dz[index] * sign;
			vertices[index1].x = vertices[index1].ox + h_tilde_dx[index].a *lambda;
			vertices[index1].z = vertices[index1].oz + h_tilde_dz[index].a *lambda;

			// 获得改顶点的法线向量
			h_tilde_slopex[index] = h_tilde_slopex[index] * sign;
			h_tilde_slopez[index] = h_tilde_slopez[index] * sign;
			n = glm::vec3(0.0f - h_tilde_slopex[index].a, 1.0f, 0.0f - h_tilde_slopez[index].a);
			n = glm::normalize(n);
			vertices[index1].nx = n.x;
			vertices[index1].ny = n.y;
			vertices[index1].nz = n.z;

			// 对于边界顶点需要特殊处理
			if (n_prime == 0 && m_prime == 0) 
			{
				vertices[index1 + N + Nplus1 * N].y = h_tilde[index].a;

				vertices[index1 + N + Nplus1 * N].x = vertices[index1 + N + Nplus1 * N].ox + h_tilde_dx[index].a *lambda;
				vertices[index1 + N + Nplus1 * N].z = vertices[index1 + N + Nplus1 * N].oz + h_tilde_dz[index].a *lambda;

				vertices[index1 + N + Nplus1 * N].nx = n.x;
				vertices[index1 + N + Nplus1 * N].ny = n.y;
				vertices[index1 + N + Nplus1 * N].nz = n.z;
			}
			if (n_prime == 0) 
			{
				vertices[index1 + N].y = h_tilde[index].a;

				vertices[index1 + N].x = vertices[index1 + N].ox + h_tilde_dx[index].a *lambda;
				vertices[index1 + N].z = vertices[index1 + N].oz + h_tilde_dz[index].a *lambda;

				vertices[index1 + N].nx = n.x;
				vertices[index1 + N].ny = n.y;
				vertices[index1 + N].nz = n.z;
			}
			if (m_prime == 0) 
			{
				vertices[index1 + Nplus1 * N].y = h_tilde[index].a;

				vertices[index1 + Nplus1 * N].x = vertices[index1 + Nplus1 * N].ox + h_tilde_dx[index].a *lambda;
				vertices[index1 + Nplus1 * N].z = vertices[index1 + Nplus1 * N].oz + h_tilde_dz[index].a *lambda;

				vertices[index1 + Nplus1 * N].nx = n.x;
				vertices[index1 + Nplus1 * N].ny = n.y;
				vertices[index1 + Nplus1 * N].nz = n.z;
			}
		}
	}
}

void OceanMesh::render(float t, glm::vec3 light_pos, glm::mat4 Projection, glm::mat4 View, glm::mat4 Model, Shader ThisRender)
{
	evaluateWavesFFT(t);

	GLuint projection, view, model;

	ThisRender.Use();

	if (VAO == 0)
	{
		glGenVertexArrays(1, &VAO);
	}
	glBindVertexArray(VAO);
	if (VBO == 0)
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_ocean)*(Nplus1)*(Nplus1), vertices, GL_DYNAMIC_DRAW);
	}
	if (EBO == 0)
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_ocean) * Nplus1 * Nplus1, vertices);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_ocean), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_ocean), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_ocean), (GLvoid*)(6 * sizeof(GLfloat)));
	glBindVertexArray(0);

	projection = glGetUniformLocation(ThisRender.Program, "Projection");
	view = glGetUniformLocation(ThisRender.Program, "View");
	model = glGetUniformLocation(ThisRender.Program, "Model");

	glUseProgram(ThisRender.Program);
	glUniformMatrix4fv(projection, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(view, 1, GL_FALSE, glm::value_ptr(View));
	glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(Model));

	glBindVertexArray(VAO);
	for (int j = 0; j < 10; j++) 
	{
		for (int i = 0; i < 10; i++) 
		{
			Model = glm::scale(glm::mat4(1.0f), glm::vec3(5.f, 5.f, 5.f));
			Model = glm::translate(Model, glm::vec3(length * i, 0, length * -j));
			glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(Model));
			glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, 0);
		}
	}
}