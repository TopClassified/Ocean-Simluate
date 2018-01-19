
#include<GLFW/glfw3.h>

#include<glm\glm.hpp>

#include<glm\gtc\matrix_transform.hpp>

#include<glm\gtc\type_ptr.hpp>

#define GL_PI 3.1415926535897932384626433832795f

#define GL_VERTICES_FACTOR 4

#define GL_MAX_VERTICES 1048576

#define GL_MAX_INDICES  (GL_MAX_VERTICES*GL_VERTICES_FACTOR)

typedef struct GLUSshapes
{
	//����
	glm::vec4 *vertices;
	//����
	glm::vec3 *normals;
	//���߿ռ��T��
	glm::vec3 *tangents;
	//���߿ؼ���B��
	glm::vec3 *bitangents;
	//��������
	glm::vec2 *texCoords;

	GLfloat *allAttributes;

	//����
	GLuint *indices;
	//��������
	GLuint numberVertices;
	//��������
	GLuint numberIndices;

	/**
	* ����������Ƭ�ķ�ʽ
	*
	* GLUS_TRIANGLES
	* GLUS_TRIANGLE_STRIP
	*/
	GLuint mode;

} GLUSshape;

/**
* ����һ����άƽ��
* halfExtend��ʾ���ĵ㵽ĳһ���ߵľ���
*/
GLboolean CreatePlane(GLUSshape* shape, const GLfloat halfExtend);

/**
* ����һ��������ƽ��
*horizontalExtend ��
*verticalExtend �߳�
*/
GLboolean CreateRectangularPlane(GLUSshape* shape, const GLfloat horizontalExtend, const GLfloat verticalExtend);

/**
* ��������������ƽ��
* horizontalExtend ��
* verticalExtend �߳�
* rows ���������
* columns ���������
* triangleStrip ���Ҫʹ��triangleStrip������Ϊtrue
*/
GLboolean CreateRectangularGridPlane(GLUSshape* shape, const GLfloat horizontalExtend, const GLfloat verticalExtend, const GLuint rows, const GLuint columns, const GLboolean triangleStrip);

/**
* ����Բ����
* radius �뾶
* numberSectors ��������������Խ��ԭ��ԽԲ
*/
GLboolean CreateDisc(GLUSshape* shape, const GLfloat radius, const GLuint numberSectors);

/**
* ����������
* halfExtend ���ĵ㵽����ƽ��ľ���
*/
GLboolean CreateCube(GLUSshape* shape, const GLfloat halfExtend);

/**
* �������壬�������Ƭ��ʹ���������Բ��
* radius ����İ뾶
* numberSlices ��Ƭ������
*/
GLboolean CreateSphere(GLUSshape* shape, const GLfloat radius, const GLuint numberSlices);

/**
* ����񷶥��״���������Ƭʹ��񷶥����Բ��
* radius 񷶥�뾶
* numberSlices 񷶥����Ƭ����
*/
GLboolean CreateDome(GLUSshape* shape, const GLfloat radius, const GLuint numberSlices);

/**
* ����Բ����״
* innerRadius �ڻ��뾶
* outerRadius �⻷�뾶
* numberSlices ��Ƭ����
* numberStacks ÿ����Ƭ�����Ķ�ջԪ������
*/
GLboolean CreateTorus(GLUSshape* shape, const GLfloat innerRadius, const GLfloat outerRadius, const GLuint numberSlices, const GLuint numberStacks);

/**
* ����Բ����
* halfExtend ���ľ������ľ���
* radius ����İ뾶
* numberSlices ��Ƭ����
*/
GLboolean CreateCylinder(GLUSshape* shape, const GLfloat halfExtend, const GLfloat radius, const GLuint numberSlices);

/**
* ����Բ׶
* halfExtend ���ĵ�������ľ���
* radius ����Բ�İ뾶
* numberSlices ��Ƭ������
* numberStacks ��ջ������
*/
GLboolean CreateCone(GLUSshape* shape, const GLfloat halfExtend, const GLfloat radius, const GLuint numberSlices, const GLuint numberStacks);

/**
* ���������������ڼ������߿ռ�ı任����
*/
GLboolean CalculateTangentBitangent(GLUSshape* shape);

/**
* ��״��������
*/
GLboolean ShapeCopyf(GLUSshape* shape, const GLUSshape* source);

/**
* ͨ���ͷ��ڴ�ɾ����״
*/
void ShapeDestroy(GLUSshape* shape);
