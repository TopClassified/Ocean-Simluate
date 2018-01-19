#pragma once
#include<cmath>
#include"Complex.h"

class FFT
{
public:
	FFT(unsigned int N);
	~FFT();

	//后log2X位进行取反操作获得与之配对的数字
	unsigned int reverse(unsigned int x);

	//获得单位向量（复数）
	Complex GetOmega(unsigned int x, unsigned int N);


	void CalculateFFT(Complex* input, Complex* output, int stride, int offset);
private:
	//多项式的项数
	unsigned int N;  
	//c数组的辅助变量用来区分上次迭代的结果和本次迭代的结果
	unsigned int LastOrNow; 
	//所需要的二进制位数（例如有8项则需要3位二进制）也是迭代次数
	unsigned int log2N;
	//常用系数2*PI
	float pi2;			
	unsigned int *reversed; 
	//存储预计算出来的单位向量
	Complex **Omega;
	Complex *c[2];
};

