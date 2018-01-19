#include"GPUFFT.h"

static GLboolean FourierIsPowerOfTwo(const GLint n)
{
	GLint test = n;

	if (test < 1)
	{
		return GL_FALSE;
	}

	while (!(test & 0x1))
	{
		test = test >> 1;
	}

	return test == 1;
}
/*
static void FourierRecursiveFunctionFFT(Complex* vector, const GLint n, GLint offset)
{
	if (n > 1)
	{
		GLint i, k;

		GLint m = n / 2;

		Complex temp;
		for (i = 1; i < m; i++)
		{
			for (k = 0; k < m - i; k++)
			{
				temp = vector[offset + 2 * k + i];
				vector[offset + 2 * k + i] = vector[offset + 2 * k + 1 + i];
				vector[offset + 2 * k + 1 + i] = temp;
			}
		}

		glusFourierRecursiveFunctionFFTc(vector, m, offset);
		glusFourierRecursiveFunctionFFTc(vector, m, offset + m);

		Complex currentW;
		currentW.a = 1.0f;
		currentW.b = 0.0f;

		Complex w;
		glusComplexRootOfUnityc(&w, n, 1, 1.0f);

		for (i = 0; i < m; i++)
		{
			Complex multiply;
			Complex addition;
			Complex subtraction;

			glusComplexMultiplyComplexc(&multiply, &currentW, &vector[offset + i + m]);

			glusComplexAddComplexc(&addition, &vector[offset + i], &multiply);
			glusComplexSubtractComplexc(&subtraction, &vector[offset + i], &multiply);

			vector[offset + i] = addition;
			vector[offset + i + m] = subtraction;

			glusComplexMultiplyComplexc(&currentW, &currentW, &w);
		}
	}
	else
	{
		// c0 = v0, so do nothing.
	}
}

GLboolean FourierRecursiveFFT(Complex* result, const Complex* vector, const GLint n)
{
	if (!result || !vector)
	{
		return GL_FALSE;
	}

	if (FourierIsPowerOfTwo(n))
	{
		GLfloat scalar = 1.0f / (GLfloat)n;

		glusVectorNCopyc(result, vector, n);

		glusVectorNConjugatec(result, result, n);

		glusFourierRecursiveFunctionFFTc(result, n, 0);

		glusVectorNConjugatec(result, result, n);
		glusVectorNMultiplyScalarc(result, result, n, scalar);

		return GL_TRUE;
	}

	return GL_FALSE;
}

GLboolean FourierRecursiveInverseFFT(Complex* result, const Complex* vector, const GLint n)
{
	if (!result || !vector)
	{
		return GL_FALSE;
	}

	if (FourierIsPowerOfTwo(n))
	{
		glusVectorNCopyc(result, vector, n);

		FourierRecursiveFunctionFFT(result, n, 0);

		return GL_TRUE;
	}

	return GL_FALSE;
}

static void FourierButterflyShuffleFunctionFFT(Complex* vector, const GLint n, const GLint offset)
{
	if (n > 1)
	{
		GLint i, k;

		GLint m = n / 2;

		Complex temp;
		for (i = 1; i < m; i++)
		{
			for (k = 0; k < m - i; k++)
			{
				temp = vector[offset + 2 * k + i];
				vector[offset + 2 * k + i] = vector[offset + 2 * k + 1 + i];
				vector[offset + 2 * k + 1 + i] = temp;
			}
		}

		FourierButterflyShuffleFunctionFFT(vector, m, offset);
		FourierButterflyShuffleFunctionFFT(vector, m, offset + m);
	}
	else
	{
		// c0 = v0, so do nothing.
	}
}

GLboolean FourierButterflyShuffleFFT(Complex* result, const Complex* vector, const GLint n)
{
	if (!result || !vector)
	{
		return GL_FALSE;
	}

	if (FourierIsPowerOfTwo(n))
	{
		glusVectorNCopyc(result, vector, n);

		FourierButterflyShuffleFunctionFFT(result, n, 0);

		return GL_TRUE;
	}

	return GL_FALSE;
}

static void FourierButterflyFunctionFFT(Complex* vector, const GLint n, const GLint offset)
{
	GLint currentStep;
	GLint currentSection;
	GLint currentButterfly;

	GLint numberSections;
	GLint numberButterfliesInSection;

	GLint m = n / 2;

	GLint steps = 0;
	GLint temp = n;
	while (!(temp & 0x1))
	{
		temp = temp >> 1;
		steps++;
	}

	numberSections = m;
	numberButterfliesInSection = 1;

	for (currentStep = 0; currentStep < steps; currentStep++)
	{
		for (currentSection = 0; currentSection < numberSections; currentSection++)
		{
			Complex currentW;
			currentW.a = 1.0f;
			currentW.b = 0.0f;

			Complex w;
			glusComplexRootOfUnityc(&w, numberButterfliesInSection * 2, 1, 1.0f);

			for (currentButterfly = 0; currentButterfly < numberButterfliesInSection; currentButterfly++)
			{
				GLint leftIndex = currentButterfly + currentSection * numberButterfliesInSection * 2;
				GLint rightIndex = currentButterfly + numberButterfliesInSection + currentSection * numberButterfliesInSection * 2;

				Complex multiply;
				Complex addition;
				Complex subtraction;

				glusComplexMultiplyComplexc(&multiply, &currentW, &vector[rightIndex]);

				glusComplexAddComplexc(&addition, &vector[leftIndex], &multiply);
				glusComplexSubtractComplexc(&subtraction, &vector[leftIndex], &multiply);

				vector[leftIndex] = addition;
				vector[rightIndex] = subtraction;

				glusComplexMultiplyComplexc(&currentW, &currentW, &w);
			}
		}

		numberButterfliesInSection *= 2;
		numberSections /= 2;
	}
}

GLboolean FourierButterflyFFT(Complex* result, const Complex* vector, const GLint n)
{
	if (!result || !vector)
	{
		return GL_FALSE;
	}

	if (FourierIsPowerOfTwo(n))
	{
		GLfloat scalar = 1.0f / (GLfloat)n;

		glusVectorNCopyc(result, vector, n);

		glusVectorNConjugatec(result, result, n);

		glusFourierButterflyShuffleFFTc(result, result, n);

		glusFourierButterflyFunctionFFTc(result, n, 0);

		glusVectorNConjugatec(result, result, n);
		glusVectorNMultiplyScalarc(result, result, n, scalar);

		return GL_TRUE;
	}

	return GL_FALSE;
}

GLboolean FourierButterflyInverseFFT(Complex* result, const Complex* vector, const GLint n)
{
	if (!result || !vector)
	{
		return GL_FALSE;
	}

	if (FourierIsPowerOfTwo(n))
	{
		glusVectorNCopyc(result, vector, n);

		glusFourierButterflyShuffleFFTc(result, result, n);

		glusFourierButterflyFunctionFFTc(result, n, 0);

		return GL_TRUE;
	}

	return GL_FALSE;
}
*/
static void FourierButterflyShuffleFunctionFFTi(GLint* vector, const GLint n, const GLint offset)
{
	if (n > 1)
	{
		GLint i, k;

		GLint m = n / 2;

		GLint temp;
		for (i = 1; i < m; i++)
		{
			for (k = 0; k < m - i; k++)
			{
				temp = vector[offset + 2 * k + i];
				vector[offset + 2 * k + i] = vector[offset + 2 * k + 1 + i];
				vector[offset + 2 * k + 1 + i] = temp;
			}
		}

		FourierButterflyShuffleFunctionFFTi(vector, m, offset);
		FourierButterflyShuffleFunctionFFTi(vector, m, offset + m);
	}
	else
	{
		// c0 = v0, so do nothing.
	}
}

GLboolean FourierButterflyShuffleFFTi(GLint* result, const GLint* vector, const GLint n)
{
	if (!result || !vector)
	{
		return GL_FALSE;
	}

	if (FourierIsPowerOfTwo(n))
	{
		GLint i;

		for (i = 0; i < n; i++)
		{
			result[i] = vector[i];
		}

		FourierButterflyShuffleFunctionFFTi(result, n, 0);

		return GL_TRUE;
	}

	return GL_FALSE;
}

