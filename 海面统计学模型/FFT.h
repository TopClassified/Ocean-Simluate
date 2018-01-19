#pragma once
#include<cmath>
#include"Complex.h"

class FFT
{
public:
	FFT(unsigned int N);
	~FFT();

	//��log2Xλ����ȡ�����������֮��Ե�����
	unsigned int reverse(unsigned int x);

	//��õ�λ������������
	Complex GetOmega(unsigned int x, unsigned int N);


	void CalculateFFT(Complex* input, Complex* output, int stride, int offset);
private:
	//����ʽ������
	unsigned int N;  
	//c����ĸ����������������ϴε����Ľ���ͱ��ε����Ľ��
	unsigned int LastOrNow; 
	//����Ҫ�Ķ�����λ����������8������Ҫ3λ�����ƣ�Ҳ�ǵ�������
	unsigned int log2N;
	//����ϵ��2*PI
	float pi2;			
	unsigned int *reversed; 
	//�洢Ԥ��������ĵ�λ����
	Complex **Omega;
	Complex *c[2];
};

