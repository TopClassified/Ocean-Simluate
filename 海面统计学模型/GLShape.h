
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
	//顶点
	glm::vec4 *vertices;
	//法线
	glm::vec3 *normals;
	//切线空间的T轴
	glm::vec3 *tangents;
	//切线控件的B轴
	glm::vec3 *bitangents;
	//纹理坐标
	glm::vec2 *texCoords;

	GLfloat *allAttributes;

	//索引
	GLuint *indices;
	//顶点数量
	GLuint numberVertices;
	//索引数量
	GLuint numberIndices;

	/**
	* 绘制三角面片的方式
	*
	* GLUS_TRIANGLES
	* GLUS_TRIANGLE_STRIP
	*/
	GLuint mode;

} GLUSshape;

/**
* 创建一个二维平面
* halfExtend表示中心点到某一条边的距离
*/
GLboolean CreatePlane(GLUSshape* shape, const GLfloat halfExtend);

/**
* 创建一个长方形平面
*horizontalExtend 宽长
*verticalExtend 高长
*/
GLboolean CreateRectangularPlane(GLUSshape* shape, const GLfloat horizontalExtend, const GLfloat verticalExtend);

/**
* 创建长方形网格平面
* horizontalExtend 宽长
* verticalExtend 高长
* rows 网格的行数
* columns 网格的列数
* triangleStrip 如果要使用triangleStrip则设置为true
*/
GLboolean CreateRectangularGridPlane(GLUSshape* shape, const GLfloat horizontalExtend, const GLfloat verticalExtend, const GLuint rows, const GLuint columns, const GLboolean triangleStrip);

/**
* 创建圆盘形
* radius 半径
* numberSectors 扇区数量，扇区越多原盘越圆
*/
GLboolean CreateDisc(GLUSshape* shape, const GLfloat radius, const GLuint numberSectors);

/**
* 创建立方体
* halfExtend 中心点到任意平面的距离
*/
GLboolean CreateCube(GLUSshape* shape, const GLfloat halfExtend);

/**
* 创建球体，更多的切片会使得球体更加圆滑
* radius 球体的半径
* numberSlices 切片的数量
*/
GLboolean CreateSphere(GLUSshape* shape, const GLfloat radius, const GLuint numberSlices);

/**
* 创建穹顶形状，更多的切片使得穹顶更加圆滑
* radius 穹顶半径
* numberSlices 穹顶的切片数量
*/
GLboolean CreateDome(GLUSshape* shape, const GLfloat radius, const GLuint numberSlices);

/**
* 创建圆环形状
* innerRadius 内环半径
* outerRadius 外环半径
* numberSlices 切片数量
* numberStacks 每个切片包含的堆栈元素数量
*/
GLboolean CreateTorus(GLUSshape* shape, const GLfloat innerRadius, const GLfloat outerRadius, const GLuint numberSlices, const GLuint numberStacks);

/**
* 创建圆柱体
* halfExtend 中心距离底面的距离
* radius 底面的半径
* numberSlices 切片数量
*/
GLboolean CreateCylinder(GLUSshape* shape, const GLfloat halfExtend, const GLfloat radius, const GLuint numberSlices);

/**
* 创建圆锥
* halfExtend 中心点距离底面的距离
* radius 底面圆的半径
* numberSlices 切片的数量
* numberStacks 堆栈的数量
*/
GLboolean CreateCone(GLUSshape* shape, const GLfloat halfExtend, const GLfloat radius, const GLuint numberSlices, const GLuint numberStacks);

/**
* 创造切向量（用于计算切线空间的变换矩阵）
*/
GLboolean CalculateTangentBitangent(GLUSshape* shape);

/**
* 形状拷贝函数
*/
GLboolean ShapeCopyf(GLUSshape* shape, const GLUSshape* source);

/**
* 通过释放内存删除形状
*/
void ShapeDestroy(GLUSshape* shape);
