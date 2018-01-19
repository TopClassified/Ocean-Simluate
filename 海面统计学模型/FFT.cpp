#include "FFT.h"



FFT::FFT(unsigned int N) : N(N), reversed(0), Omega(0), pi2(2 * 3.1415926f)
{
	c[0] = c[1] = 0;

	log2N = log(N) / log(2);

	// Ԥ�����ÿ�����ֶ�Ӧ���������
	reversed = new unsigned int[N];		
	for (int i = 0; i < N; i++)
	{
		reversed[i] = reverse(i);
	}

	//Ԥ�������ÿ�ε���������Ҫ�����е�λ����
	int pow2 = 1;
	Omega = new Complex *[log2N];
	for (int i = 0; i < log2N; i++) 
	{
		Omega[i] = new Complex[pow2];
		for (int j = 0; j < pow2; j++)
		{
			Omega[i][j] = GetOmega(j, pow2 * 2);
		}
		pow2 *= 2;
	}

	c[0] = new Complex[N];
	c[1] = new Complex[N];
	LastOrNow = 0;
}


FFT::~FFT()
{
	if (c[0])
	{
		delete[] c[0];
	}
	if (c[1])
	{
		delete[] c[1];
	}
	if (Omega)
	{
		for (int i = 0; i < log2N; i++)
		{
			if (Omega[i])
			{
				delete[] Omega[i];
			}
		}
		delete[] Omega;
	}
	if (reversed)
	{
		delete[] reversed;
	}
}

unsigned int FFT::reverse(unsigned int x) 
{
	unsigned int res = 0;
	for (int i= 0; i < log2N; i++) 
	{
		res = (res << 1) + (x & 1);
		x >>= 1;
	}
	return res;
}

Complex FFT::GetOmega(unsigned int x, unsigned int N)
{
	return Complex(cos(pi2 * x / N), sin(pi2 * x / N));
}

void FFT::CalculateFFT(Complex* input, Complex* output, int stride, int offset) 
{
	for (int i = 0; i < N; i++)
	{
		//input�洢�����ÿ������������Ϣ��stride��offset���������������
		c[LastOrNow][i] = input[reversed[i] * stride + offset];
	}
	//�ôε�������Ҫ�ļ����������N/2,N/4......
	int loops = N >> 1;
	int TheSize = 1 << 1;
	int HalfSize = 1;
	//��¼��������
	int cnt = 0;
	//�ܵ�������
	for (int i = 1; i <= log2N; i++) 
	{
		LastOrNow ^= 1;
		//�ô�ѭ����Ҫ�������
		for (int j = 0; j < loops; j++) 
		{
			//ʹ�ù�ʽ
			//��0<=k<halfsize��ʱ��ʹ�ù�ʽ��A(w(k,n))=A1(w(k,n/2))+w(k,n)*A2(w(k,n/2))
			for (int k = 0; k < HalfSize; k++)
			{
				c[LastOrNow][TheSize * j + k] = c[LastOrNow ^ 1][TheSize * j + k] + c[LastOrNow ^ 1][TheSize * j + HalfSize + k] * Omega[cnt][k];
			}
			//��halfsize<=k<n��ʱ��ʹ�ù�ʽ��A1(w(k,n/2))-w(k,n)*A2(w(k,n/2))
			for (int k = HalfSize; k < TheSize; k++)
			{
				c[LastOrNow][TheSize * j + k] = c[LastOrNow ^ 1][TheSize * j - HalfSize + k] - c[LastOrNow ^ 1][TheSize * j + k] * Omega[cnt][k - HalfSize];
			}
		}
		loops >>= 1;
		TheSize <<= 1;
		HalfSize <<= 1;
		cnt++;
	}

	for (int i = 0; i < N; i++)
	{
		output[i * stride + offset] = c[LastOrNow][i];
	}
}