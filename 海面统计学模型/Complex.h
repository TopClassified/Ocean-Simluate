#pragma once


class Complex
{
public:
	float a, b;//复数的实部和虚部
	static unsigned int additions, multiplications;
	Complex();
	Complex(float a, float b);
	Complex GetConjugate();//求复数的共轭
	Complex operator*(const Complex& c) const;
	Complex operator+(const Complex& c) const;
	Complex operator-(const Complex& c) const;
	Complex operator-() const;
	Complex operator*(const float c) const;
	Complex& operator=(const Complex& c);
	static void reset();
	~Complex();
};

