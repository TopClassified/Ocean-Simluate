#include "FFT.h"



FFT::FFT(unsigned int N) : N(N), reversed(0), Omega(0), pi2(2 * 3.1415926f)
{
	c[0] = c[1] = 0;

	log2N = log(N) / log(2);

	// 预计算出每个数字对应的配对数字
	reversed = new unsigned int[N];		
	for (int i = 0; i < N; i++)
	{
		reversed[i] = reverse(i);
	}

	//预计算出来每次迭代，所需要的所有单位向量
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
		//input存储网格的每个顶点的相关信息，stride和offset计算出来顶点的序号
		c[LastOrNow][i] = input[reversed[i] * stride + offset];
	}
	//该次迭代所需要的计算的组数：N/2,N/4......
	int loops = N >> 1;
	int TheSize = 1 << 1;
	int HalfSize = 1;
	//记录迭代次数
	int cnt = 0;
	//总迭代次数
	for (int i = 1; i <= log2N; i++) 
	{
		LastOrNow ^= 1;
		//该次循环所要计算的组
		for (int j = 0; j < loops; j++) 
		{
			//使用公式
			//当0<=k<halfsize的时候使用公式：A(w(k,n))=A1(w(k,n/2))+w(k,n)*A2(w(k,n/2))
			for (int k = 0; k < HalfSize; k++)
			{
				c[LastOrNow][TheSize * j + k] = c[LastOrNow ^ 1][TheSize * j + k] + c[LastOrNow ^ 1][TheSize * j + HalfSize + k] * Omega[cnt][k];
			}
			//当halfsize<=k<n的时候使用公式：A1(w(k,n/2))-w(k,n)*A2(w(k,n/2))
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