#pragma once


class Complex
{
public:
	float a, b;//������ʵ�����鲿
	static unsigned int additions, multiplications;
	Complex();
	Complex(float a, float b);
	Complex GetConjugate();//�����Ĺ���
	Complex operator*(const Complex& c) const;
	Complex operator+(const Complex& c) const;
	Complex operator-(const Complex& c) const;
	Complex operator-() const;
	Complex operator*(const float c) const;
	Complex& operator=(const Complex& c);
	static void reset();
	~Complex();
};

