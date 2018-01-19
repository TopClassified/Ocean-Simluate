#include<GLFW\glfw3.h>
#include"Complex.h"

/**
* Performs a fast fourier transform on a given vector with N elements, using a recursive algorithm.
*
* @param result The transformed vector.
* @param vector The source vector.
* @param n 	 The number of elements.
*
* @return GLUS_TRUE, if transform succeeded.
*/
GLboolean FourierRecursiveFFT(Complex* result, const Complex* vector, const GLint n);

/**
* Performs an inverse fast fourier transform on a given vector with N elements, using a recursive algorithm.
*
* @param result The transformed vector.
* @param vector The source vector.
* @param n 	 The number of elements.
*
* @return GLUS_TRUE, if transform succeeded.
*/
GLboolean FourierRecursiveInverseFFT(Complex* result, const Complex* vector, const GLint n);

/**
* Shuffles a vector with N elements, that it can be used for a FFT butterfly algorithm.
*
* @param result The transformed vector.
* @param vector The source vector.
* @param n 	 The number of elements.
*
* @return GLUS_TRUE, if shuffle succeeded.
*/
GLboolean FourierButterflyShuffleFFT(Complex* result, const Complex* vector, const GLint n);

/**
* Shuffles a index vector with N elements, that the indices can be used for a FFT butterfly algorithm.
*
* @param result The transformed vector.
* @param vector The source vector.
* @param n 	 The number of elements.
*
* @return GLUS_TRUE, if shuffle succeeded.
*/
GLboolean FourierButterflyShuffleFFTi(GLint* result, const GLint* vector, const GLint n);

/**
* Performs a fast fourier transform on a given vector with N elements, using a butterfly algorithm.
* Shuffling of the elements is done in this function.
*
* @param result The transformed vector.
* @param vector The source vector.
* @param n 	 The number of elements.
*
* @return GLUS_TRUE, if transform succeeded.
*/
GLboolean FourierButterflyFFT(Complex* result, const Complex* vector, const GLint n);

/**
* Performs an inverse fast fourier transform on a given vector with N elements, using a butterfly algorithm.
* Shuffling of the elements is done in this function.
*
* @param result The transformed vector.
* @param vector The source vector.
* @param n 	 The number of elements.
*
* @return GLUS_TRUE, if transform succeeded.
*/
GLboolean FourierButterflyInverseFFT(Complex* result, const Complex* vector, const GLint n);

