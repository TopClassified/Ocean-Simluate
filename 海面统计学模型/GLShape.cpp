#include "GLShape.h"
#include <string.h>

void ShapeInit(GLUSshape* shape)
{
	if (!shape)
	{
		return;
	}
	memset(shape, 0, sizeof(GLUSshape));
	shape->mode = 0;
}

GLboolean ShapeCheck(GLUSshape* shape)
{
	if (!shape)
	{
		return GL_FALSE;
	}

	return shape->vertices && shape->normals && shape->tangents && shape->texCoords && shape->indices;
}

GLboolean ShapeFinalize(GLUSshape* shape)
{
	GLuint i;

	// vertex, normal, tangent, bitangent, texCoords
	GLuint stride = 4 + 3 + 3 + 3 + 2;

	if (!shape)
	{
		return GL_FALSE;
	}

	// Add bitangents
	shape->bitangents = new glm::vec3[shape->numberVertices];

	if (!shape->bitangents)
	{
		return GL_FALSE;
	}

	for (i = 0; i < shape->numberVertices; i++)
	{
		shape->bitangents[i] = glm::cross(shape->normals[i], shape->tangents[i]); 
	}

	shape->allAttributes = new GLfloat[(stride * shape->numberVertices * sizeof(GLfloat))];

	if (!shape->allAttributes)
	{
		return GL_FALSE;
	}

	for (i = 0; i < shape->numberVertices; i++)
	{
		shape->allAttributes[i * stride + 0] = shape->vertices[i].x;
		shape->allAttributes[i * stride + 1] = shape->vertices[i].y;
		shape->allAttributes[i * stride + 2] = shape->vertices[i].z;
		shape->allAttributes[i * stride + 3] = shape->vertices[i].w;

		shape->allAttributes[i * stride + 4] = shape->normals[i].x;
		shape->allAttributes[i * stride + 5] = shape->normals[i].y;
		shape->allAttributes[i * stride + 6] = shape->normals[i].z;

		shape->allAttributes[i * stride + 7] = shape->tangents[i].x;
		shape->allAttributes[i * stride + 8] = shape->tangents[i].y;
		shape->allAttributes[i * stride + 9] = shape->tangents[i].z;

		shape->allAttributes[i * stride + 10] = shape->bitangents[i].x;
		shape->allAttributes[i * stride + 11] = shape->bitangents[i].y;
		shape->allAttributes[i * stride + 12] = shape->bitangents[i].z;

		shape->allAttributes[i * stride + 13] = shape->texCoords[i].x;
		shape->allAttributes[i * stride + 14] = shape->texCoords[i].y;
	}

	return GL_TRUE;
}

GLboolean CreatePlane(GLUSshape* shape, const GLfloat halfExtend)
{
	GLuint i;

	GLuint numberVertices = 4;
	GLuint numberIndices = 6;

	GLfloat xy_vertices[] = { -1.0f, -1.0f, 0.0f, +1.0f, +1.0f, -1.0f, 0.0f, +1.0f, -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, +1.0f, 0.0f, +1.0f };

	GLfloat xy_normals[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

	GLfloat xy_tangents[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

	GLfloat xy_texCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	GLuint xy_indices[] = { 0, 1, 2, 1, 3, 2 };

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices]; 
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeCheck(shape);
		return GL_FALSE;
	}

	memcpy(shape->vertices, xy_vertices, sizeof(xy_vertices));
	for (i = 0; i < numberVertices; i++)
	{
		shape->vertices[i].x *= halfExtend;
		shape->vertices[i].y *= halfExtend;
	}

	memcpy(shape->normals, xy_normals, sizeof(xy_normals));

	memcpy(shape->tangents, xy_tangents, sizeof(xy_tangents));

	memcpy(shape->texCoords, xy_texCoords, sizeof(xy_texCoords));

	memcpy(shape->indices, xy_indices, sizeof(xy_indices));

	if (!ShapeFinalize(shape))
	{
		ShapeFinalize(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CreateRectangularPlane(GLUSshape* shape, const GLfloat horizontalExtend, const GLfloat verticalExtend)
{
	GLuint i;

	GLuint numberVertices = 4;
	GLuint numberIndices = 6;

	GLfloat xy_vertices[] = { -1.0f, -1.0f, 0.0f, +1.0f, +1.0f, -1.0f, 0.0f, +1.0f, -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, +1.0f, 0.0f, +1.0f };

	GLfloat xy_normals[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

	GLfloat xy_tangents[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

	GLfloat xy_texCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	GLuint xy_indices[] = { 0, 1, 2, 1, 3, 2 };

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);
		return GL_FALSE;
	}

	memcpy(shape->vertices, xy_vertices, sizeof(xy_vertices));
	for (i = 0; i < numberVertices; i++)
	{
		shape->vertices[i * 4 + 0] *= horizontalExtend;
		shape->vertices[i * 4 + 1] *= verticalExtend;
	}

	memcpy(shape->normals, xy_normals, sizeof(xy_normals));

	memcpy(shape->tangents, xy_tangents, sizeof(xy_tangents));

	memcpy(shape->texCoords, xy_texCoords, sizeof(xy_texCoords));

	memcpy(shape->indices, xy_indices, sizeof(xy_indices));

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CreateRectangularGridPlane(GLUSshape* shape, const GLfloat horizontalExtend, const GLfloat verticalExtend, const GLuint rows, const GLuint columns, const GLboolean triangleStrip)
{
	GLuint i, currentRow, currentColumn;

	GLuint numberVertices = (rows + 1) * (columns + 1);
	GLuint numberIndices;

	GLfloat x, y, s, t;

	if (triangleStrip)
	{
		numberIndices = rows * 2 * (columns + 1);
	}
	else
	{
		numberIndices = rows * 6 * columns;
	}

	if (rows < 1 || columns < 1 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	if (triangleStrip)
	{
		shape->mode = 1;
	}
	else
	{
		shape->mode = 0;
	}

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	for (i = 0; i < numberVertices; i++)
	{
		x = (GLfloat)(i % (columns + 1)) / (GLfloat)columns;
		y = 1.0f - (GLfloat)(i / (columns + 1)) / (GLfloat)rows;

		s = x;
		t = y;

		shape->vertices[i].x = horizontalExtend * (x - 0.5f);
		shape->vertices[i].y = verticalExtend * (y - 0.5f);
		shape->vertices[i].z = 0.0f;
		shape->vertices[i].w = 1.0f;

		shape->normals[i].x = 0.0f;
		shape->normals[i].y = 0.0f;
		shape->normals[i].z = 1.0f;

		shape->tangents[i].x = 1.0f;
		shape->tangents[i].y = 0.0f;
		shape->tangents[i].z = 0.0f;

		shape->texCoords[i].x = s;
		shape->texCoords[i].y = t;
	}

	if (triangleStrip)
	{
		for (i = 0; i < rows * (columns + 1); i++)
		{
			currentColumn = i % (columns + 1);
			currentRow = i / (columns + 1);

			if (currentRow == 0)
			{
				// Left to right, top to bottom
				shape->indices[i * 2] = currentColumn + currentRow * (columns + 1);
				shape->indices[i * 2 + 1] = currentColumn + (currentRow + 1) * (columns + 1);
			}
			else
			{
				// Right to left, bottom to up
				shape->indices[i * 2] = (columns - currentColumn) + (currentRow + 1) * (columns + 1);
				shape->indices[i * 2 + 1] = (columns - currentColumn) + currentRow * (columns + 1);
			}
		}
	}
	else
	{
		for (i = 0; i < rows * columns; i++)
		{
			currentColumn = i % columns;
			currentRow = i / columns;

			shape->indices[i * 6 + 0] = currentColumn + currentRow * (columns + 1);
			shape->indices[i * 6 + 1] = currentColumn + (currentRow + 1) * (columns + 1);
			shape->indices[i * 6 + 2] = (currentColumn + 1) + (currentRow + 1) * (columns + 1);

			shape->indices[i * 6 + 3] = (currentColumn + 1) + (currentRow + 1) * (columns + 1);
			shape->indices[i * 6 + 4] = (currentColumn + 1) + currentRow * (columns + 1);
			shape->indices[i * 6 + 5] = currentColumn + currentRow * (columns + 1);
		}
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);
		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CreateDisc(GLUSshape* shape, const GLfloat radius, const GLuint numberSectors)
{
	GLuint i;

	GLuint numberVertices = numberSectors + 2;
	GLuint numberIndices = numberSectors * 3;

	GLfloat angleStep = (2.0f * GL_PI) / ((GLfloat)numberSectors);

	GLuint indexIndices;
	GLuint indexCounter;

	GLuint vertexCounter = 0;

	if (numberSectors < 3 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	// Center
	shape->vertices[vertexCounter].x = 0.0f;
	shape->vertices[vertexCounter].y = 0.0f;
	shape->vertices[vertexCounter].z = 0.0f;
	shape->vertices[vertexCounter].w = 1.0f;

	shape->normals[vertexCounter].x = 0.0f;
	shape->normals[vertexCounter].y = 0.0f;
	shape->normals[vertexCounter].z = 1.0f;

	shape->tangents[vertexCounter].x = 1.0f;
	shape->tangents[vertexCounter].y = 0.0f;
	shape->tangents[vertexCounter].z = 0.0f;

	shape->texCoords[vertexCounter].x = 0.5f;
	shape->texCoords[vertexCounter].y = 0.5f;

	vertexCounter++;

	for (i = 0; i < numberSectors + 1; i++)
	{
		GLfloat currentAngle = angleStep * (GLfloat)i;

		shape->vertices[vertexCounter].x = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter].y = sinf(currentAngle) * radius;
		shape->vertices[vertexCounter].z = 0.0f;
		shape->vertices[vertexCounter].w = 1.0f;

		shape->normals[vertexCounter].x = 0.0f;
		shape->normals[vertexCounter].y = 0.0f;
		shape->normals[vertexCounter].z = 1.0f;

		shape->tangents[vertexCounter].x = 1.0f;
		shape->tangents[vertexCounter].y = 0.0f;
		shape->tangents[vertexCounter].z = 0.0f;

		shape->texCoords[vertexCounter].x = 0.5f * cosf(currentAngle) * 0.5f;
		shape->texCoords[vertexCounter].y = 0.5f * sinf(currentAngle) * 0.5f;

		vertexCounter++;
	}

	indexIndices = 0;

	// Bottom
	indexCounter = 1;

	for (i = 0; i < numberSectors; i++)
	{
		shape->indices[indexIndices++] = 0;
		shape->indices[indexIndices++] = indexCounter;
		shape->indices[indexIndices++] = indexCounter + 1;

		indexCounter++;
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CreateCube(GLUSshape* shape, const GLfloat halfExtend)
{
	GLuint i;

	GLuint numberVertices = 24;
	GLuint numberIndices = 36;

	GLfloat cubeVertices[] = { -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f,

		-1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f,

		-1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f,

		-1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,

		-1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f,

		+1.0f, -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f };

	GLfloat cubeNormals[] = { 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

		0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f,

		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

		0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,

		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		+1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f };

	GLfloat cubeTangents[] = { +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,

		+1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

		+1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,

		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f };

	GLfloat cubeTexCoords[] =
	{ 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

	GLuint cubeIndices[] = { 0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21 };

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	memcpy(shape->vertices, cubeVertices, sizeof(cubeVertices));
	for (i = 0; i < numberVertices; i++)
	{
		shape->vertices[i * 4 + 0] *= halfExtend;
		shape->vertices[i * 4 + 1] *= halfExtend;
		shape->vertices[i * 4 + 2] *= halfExtend;
	}

	memcpy(shape->normals, cubeNormals, sizeof(cubeNormals));

	memcpy(shape->tangents, cubeTangents, sizeof(cubeTangents));

	memcpy(shape->texCoords, cubeTexCoords, sizeof(cubeTexCoords));

	memcpy(shape->indices, cubeIndices, sizeof(cubeIndices));

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CreateSphere(GLUSshape* shape, const GLfloat radius, const GLuint numberSlices)
{
	GLuint i, j;

	GLuint numberParallels = numberSlices / 2;
	GLuint numberVertices = (numberParallels + 1) * (numberSlices + 1);
	GLuint numberIndices = numberParallels * numberSlices * 6;

	GLfloat angleStep = (2.0f * GL_PI) / ((GLfloat)numberSlices);

	GLuint indexIndices;

	// used later to help us calculating tangents vectors
	glm::vec4 helpVector = { 1.0f, 0.0f, 0.0f, 0.0f };
	glm::mat4 helpMatrix = glm::mat4();

	if (numberSlices < 3 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	for (i = 0; i < numberParallels + 1; i++)
	{
		for (j = 0; j < numberSlices + 1; j++)
		{
			GLuint vertexIndex = (i * (numberSlices + 1) + j);
			GLuint normalIndex = (i * (numberSlices + 1) + j);
			GLuint tangentIndex = (i * (numberSlices + 1) + j);
			GLuint texCoordsIndex = (i * (numberSlices + 1) + j);

			shape->vertices[vertexIndex].x = radius * sinf(angleStep * (GLfloat)i) * sinf(angleStep * (GLfloat)j);
			shape->vertices[vertexIndex].y = radius * cosf(angleStep * (GLfloat)i);
			shape->vertices[vertexIndex].z = radius * sinf(angleStep * (GLfloat)i) * cosf(angleStep * (GLfloat)j);
			shape->vertices[vertexIndex].w = 1.0f;

			shape->normals[normalIndex].x = shape->vertices[vertexIndex].x / radius;
			shape->normals[normalIndex].y = shape->vertices[vertexIndex].y / radius;
			shape->normals[normalIndex].z = shape->vertices[vertexIndex].z / radius;

			shape->texCoords[texCoordsIndex].x = (GLfloat)j / (GLfloat)numberSlices;
			shape->texCoords[texCoordsIndex].y = 1.0f - (GLfloat)i / (GLfloat)numberParallels;

			helpMatrix = glm::rotate(helpMatrix, glm::radians(360.0f * shape->texCoords[texCoordsIndex].x), glm::vec3(0.0f, 1.0f, 0.0f));
			shape->tangents[tangentIndex] = helpMatrix * helpVector;
		}
	}

	indexIndices = 0;
	for (i = 0; i < numberParallels; i++)
	{
		for (j = 0; j < numberSlices; j++)
		{
			shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
			shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + j;
			shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);

			shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
			shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);
			shape->indices[indexIndices++] = i * (numberSlices + 1) + (j + 1);
		}
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean  CreateDome(GLUSshape* shape, const GLfloat radius, const GLuint numberSlices)
{
	GLuint i, j;

	GLuint numberParallels = numberSlices / 4;
	GLuint numberVertices = (numberParallels + 1) * (numberSlices + 1);
	GLuint numberIndices = numberParallels * numberSlices * 6;

	GLfloat angleStep = (2.0f * GL_PI) / ((GLfloat)numberSlices);

	GLuint indexIndices;

	// used later to help us calculating tangents vectors
	glm::vec4 helpVector = { 1.0f, 0.0f, 0.0f, 0.0f };
	glm::mat4 helpMatrix = glm::mat4();

	if (numberSlices < 3 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	for (i = 0; i < numberParallels + 1; i++)
	{
		for (j = 0; j < numberSlices + 1; j++)
		{
			GLuint vertexIndex = (i * (numberSlices + 1) + j) * 4;
			GLuint normalIndex = (i * (numberSlices + 1) + j) * 3;
			GLuint tangentIndex = (i * (numberSlices + 1) + j) * 3;
			GLuint texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

			shape->vertices[vertexIndex].x = radius * sinf(angleStep * (GLfloat)i) * sinf(angleStep * (GLfloat)j);
			shape->vertices[vertexIndex].y = radius * cosf(angleStep * (GLfloat)i);
			shape->vertices[vertexIndex].z = radius * sinf(angleStep * (GLfloat)i) * cosf(angleStep * (GLfloat)j);
			shape->vertices[vertexIndex].w = 1.0f;

			shape->normals[normalIndex].x = shape->vertices[vertexIndex].x / radius;
			shape->normals[normalIndex].y = shape->vertices[vertexIndex].y / radius;
			shape->normals[normalIndex].z = shape->vertices[vertexIndex].z / radius;

			shape->texCoords[texCoordsIndex].x = (GLfloat)j / (GLfloat)numberSlices;
			shape->texCoords[texCoordsIndex].y = 1.0f - (GLfloat)i / (GLfloat)numberParallels;

			// use quaternion to get the tangent vector
			helpMatrix = glm::rotate(helpMatrix, glm::radians(360.0f * shape->texCoords[texCoordsIndex].x), glm::vec3(0.0f, 1.0f, 0.0f));
			shape->tangents[tangentIndex] = helpMatrix * helpVector;
		}
	}

	indexIndices = 0;
	for (i = 0; i < numberParallels; i++)
	{
		for (j = 0; j < numberSlices; j++)
		{
			shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
			shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + j;
			shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);

			shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
			shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);
			shape->indices[indexIndices++] = i * (numberSlices + 1) + (j + 1);
		}
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

/*
* @author Pablo Alonso-Villaverde Roza
* @author Norbert Nopper
*/
GLboolean  CreateTorus(GLUSshape* shape, const GLfloat innerRadius, const GLfloat outerRadius, const GLuint numberSlices, const GLuint numberStacks)
{
	// s, t = parametric values of the equations, in the range [0,1]
	GLfloat s = 0;
	GLfloat t = 0;

	// sIncr, tIncr are increment values aplied to s and t on each loop iteration to generate the torus
	GLfloat sIncr;
	GLfloat tIncr;

	// to store precomputed sin and cos values
	GLfloat cos2PIs, sin2PIs, cos2PIt, sin2PIt;

	GLuint numberVertices;
	GLuint numberIndices;

	// used later to help us calculating tangents vectors
	glm::vec4 helpVector = { 1.0f, 0.0f, 0.0f, 0.0f };
	glm::mat4 helpMatrix = glm::mat4();

	// indices for each type of buffer (of vertices, indices, normals...)
	GLuint indexVertices, indexIndices, indexNormals, indexTangents, indexTexCoords;

	// loop counters
	GLuint sideCount, faceCount;

	// used to generate the indices
	GLuint v0, v1, v2, v3;

	GLfloat torusRadius = (outerRadius - innerRadius) / 2.0f;
	GLfloat centerRadius = outerRadius - torusRadius;

	numberVertices = (numberStacks + 1) * (numberSlices + 1);
	numberIndices = numberStacks * numberSlices * 2 * 3; // 2 triangles per face * 3 indices per triangle

	if (numberSlices < 3 || numberStacks < 3 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	sIncr = 1.0f / (GLfloat)numberSlices;
	tIncr = 1.0f / (GLfloat)numberStacks;

	// generate vertices and its attributes
	for (sideCount = 0; sideCount <= numberSlices; ++sideCount, s += sIncr)
	{
		// precompute some values
		cos2PIs = (GLfloat)cosf(2.0f * GL_PI * s);
		sin2PIs = (GLfloat)sinf(2.0f * GL_PI * s);

		t = 0.0f;
		for (faceCount = 0; faceCount <= numberStacks; ++faceCount, t += tIncr)
		{
			// precompute some values
			cos2PIt = (GLfloat)cosf(2.0f * GL_PI * t);
			sin2PIt = (GLfloat)sinf(2.0f * GL_PI * t);

			// generate vertex and stores it in the right position
			indexVertices = ((sideCount * (numberStacks + 1)) + faceCount);
			shape->vertices[indexVertices].x = (centerRadius + torusRadius * cos2PIt) * cos2PIs;
			shape->vertices[indexVertices].y = (centerRadius + torusRadius * cos2PIt) * sin2PIs;
			shape->vertices[indexVertices].z = torusRadius * sin2PIt;
			shape->vertices[indexVertices].w = 1.0f;

			// generate normal and stores it in the right position
			// NOTE: cos (2PIx) = cos (x) and sin (2PIx) = sin (x) so, we can use this formula
			//       normal = {cos(2PIs)cos(2PIt) , sin(2PIs)cos(2PIt) ,sin(2PIt)}
			indexNormals = ((sideCount * (numberStacks + 1)) + faceCount);
			shape->normals[indexNormals].x = cos2PIs * cos2PIt;
			shape->normals[indexNormals].y = sin2PIs * cos2PIt;
			shape->normals[indexNormals].z = sin2PIt;

			// generate texture coordinates and stores it in the right position
			indexTexCoords = ((sideCount * (numberStacks + 1)) + faceCount);
			shape->texCoords[indexTexCoords].x = s;
			shape->texCoords[indexTexCoords].y = t;

			// use quaternion to get the tangent vector
			helpMatrix = glm::rotate(helpMatrix, glm::radians(360.0f * s), glm::vec3(0.0f, 1.0f, 0.0f));

			indexTangents = ((sideCount * (numberStacks + 1)) + faceCount);

			shape->tangents[indexTangents] = helpMatrix * helpVector;
		}
	}

	// generate indices
	indexIndices = 0;
	for (sideCount = 0; sideCount < numberSlices; ++sideCount)
	{
		for (faceCount = 0; faceCount < numberStacks; ++faceCount)
		{
			// get the number of the vertices for a face of the torus. They must be < numVertices
			v0 = ((sideCount * (numberStacks + 1)) + faceCount);
			v1 = (((sideCount + 1) * (numberStacks + 1)) + faceCount);
			v2 = (((sideCount + 1) * (numberStacks + 1)) + (faceCount + 1));
			v3 = ((sideCount * (numberStacks + 1)) + (faceCount + 1));

			// first triangle of the face, counter clock wise winding
			shape->indices[indexIndices++] = v0;
			shape->indices[indexIndices++] = v1;
			shape->indices[indexIndices++] = v2;

			// second triangle of the face, counter clock wise winding
			shape->indices[indexIndices++] = v0;
			shape->indices[indexIndices++] = v2;
			shape->indices[indexIndices++] = v3;
		}
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean ShapeCreateCylinder(GLUSshape* shape, const GLfloat halfExtend, const GLfloat radius, const GLuint numberSlices)
{
	GLuint i, j;

	GLuint numberVertices = (numberSlices + 2) * 2 + (numberSlices + 1) * 2;
	GLuint numberIndices = numberSlices * 3 * 2 + numberSlices * 6;

	GLfloat angleStep = (2.0f * GL_PI) / ((GLfloat)numberSlices);

	GLuint indexIndices;
	GLuint centerIndex;
	GLuint indexCounter;

	GLuint vertexCounter = 0;

	if (numberSlices < 3 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	// Center bottom
	shape->vertices[vertexCounter].x = 0.0f;
	shape->vertices[vertexCounter].y = -halfExtend;
	shape->vertices[vertexCounter].z = 0.0f;
	shape->vertices[vertexCounter].w = 1.0f;

	shape->normals[vertexCounter].x = 0.0f;
	shape->normals[vertexCounter].y = -1.0f;
	shape->normals[vertexCounter].z = 0.0f;

	shape->tangents[vertexCounter].x = 0.0f;
	shape->tangents[vertexCounter].y = 0.0f;
	shape->tangents[vertexCounter].z = 1.0f;

	shape->texCoords[vertexCounter].x = 0.0f;
	shape->texCoords[vertexCounter].y = 0.0f;

	vertexCounter++;

	// Bottom
	for (i = 0; i < numberSlices + 1; i++)
	{
		GLfloat currentAngle = angleStep * (GLfloat)i;

		shape->vertices[vertexCounter].x = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter].y = -halfExtend;
		shape->vertices[vertexCounter].z = -sinf(currentAngle) * radius;
		shape->vertices[vertexCounter].w = 1.0f;

		shape->normals[vertexCounter].x = 0.0f;
		shape->normals[vertexCounter].y = -1.0f;
		shape->normals[vertexCounter].z = 0.0f;

		shape->tangents[vertexCounter].x = sinf(currentAngle);
		shape->tangents[vertexCounter].y = 0.0f;
		shape->tangents[vertexCounter].z = cosf(currentAngle);

		shape->texCoords[vertexCounter].x = 0.0f;
		shape->texCoords[vertexCounter].y = 0.0f;

		vertexCounter++;
	}

	// Center top
	shape->vertices[vertexCounter].x = 0.0f;
	shape->vertices[vertexCounter].y = halfExtend;
	shape->vertices[vertexCounter].z = 0.0f;
	shape->vertices[vertexCounter].w = 1.0f;

	shape->normals[vertexCounter].x = 0.0f;
	shape->normals[vertexCounter].y = 1.0f;
	shape->normals[vertexCounter].z = 0.0f;

	shape->tangents[vertexCounter].x = 0.0f;
	shape->tangents[vertexCounter].y = 0.0f;
	shape->tangents[vertexCounter].z = -1.0f;

	shape->texCoords[vertexCounter].x = 1.0f;
	shape->texCoords[vertexCounter].y = 1.0f;

	vertexCounter++;

	// Top
	for (i = 0; i < numberSlices + 1; i++)
	{
		GLfloat currentAngle = angleStep * (GLfloat)i;

		shape->vertices[vertexCounter].x = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter].y = halfExtend;
		shape->vertices[vertexCounter].z = -sinf(currentAngle) * radius;
		shape->vertices[vertexCounter].w = 1.0f;

		shape->normals[vertexCounter].x = 0.0f;
		shape->normals[vertexCounter].y = 1.0f;
		shape->normals[vertexCounter].z = 0.0f;

		shape->tangents[vertexCounter].x = -sinf(currentAngle);
		shape->tangents[vertexCounter].y = 0.0f;
		shape->tangents[vertexCounter].z = -cosf(currentAngle);

		shape->texCoords[vertexCounter].x = 1.0f;
		shape->texCoords[vertexCounter].y = 1.0f;

		vertexCounter++;
	}

	for (i = 0; i < numberSlices + 1; i++)
	{
		GLfloat currentAngle = angleStep * (GLfloat)i;

		GLfloat sign = -1.0f;

		for (j = 0; j < 2; j++)
		{
			shape->vertices[vertexCounter].x = cosf(currentAngle) * radius;
			shape->vertices[vertexCounter].y = halfExtend * sign;
			shape->vertices[vertexCounter].z = -sinf(currentAngle) * radius;
			shape->vertices[vertexCounter].w = 1.0f;

			shape->normals[vertexCounter].x = cosf(currentAngle);
			shape->normals[vertexCounter].y = 0.0f;
			shape->normals[vertexCounter].z = -sinf(currentAngle);

			shape->tangents[vertexCounter].x = -sinf(currentAngle);
			shape->tangents[vertexCounter].y = 0.0f;
			shape->tangents[vertexCounter].z = -cosf(currentAngle);

			shape->texCoords[vertexCounter].x = (GLfloat)i / (GLfloat)numberSlices;
			shape->texCoords[vertexCounter].y = (sign + 1.0f) / 2.0f;

			vertexCounter++;

			sign = 1.0f;
		}
	}

	indexIndices = 0;

	// Bottom
	centerIndex = 0;
	indexCounter = 1;

	for (i = 0; i < numberSlices; i++)
	{
		shape->indices[indexIndices++] = centerIndex;
		shape->indices[indexIndices++] = indexCounter + 1;
		shape->indices[indexIndices++] = indexCounter;

		indexCounter++;
	}
	indexCounter++;

	// Top
	centerIndex = indexCounter;
	indexCounter++;

	for (i = 0; i < numberSlices; i++)
	{
		shape->indices[indexIndices++] = centerIndex;
		shape->indices[indexIndices++] = indexCounter;
		shape->indices[indexIndices++] = indexCounter + 1;

		indexCounter++;
	}
	indexCounter++;

	// Sides
	for (i = 0; i < numberSlices; i++)
	{
		shape->indices[indexIndices++] = indexCounter;
		shape->indices[indexIndices++] = indexCounter + 2;
		shape->indices[indexIndices++] = indexCounter + 1;

		shape->indices[indexIndices++] = indexCounter + 2;
		shape->indices[indexIndices++] = indexCounter + 3;
		shape->indices[indexIndices++] = indexCounter + 1;

		indexCounter += 2;
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CreateCone(GLUSshape* shape, const GLfloat halfExtend, const GLfloat radius, const GLuint numberSlices, const GLuint numberStacks)
{
	GLuint i, j;

	GLuint numberVertices = (numberSlices + 2) + (numberSlices + 1) * (numberStacks + 1);
	GLuint numberIndices = numberSlices * 3 + numberSlices * 6 * numberStacks;

	GLfloat angleStep = (2.0f * GL_PI) / ((GLfloat)numberSlices);

	GLuint indexIndices;
	GLuint centerIndex;
	GLuint indexCounter;

	GLuint vertexCounter = 0;

	GLfloat h = 2.0f * halfExtend;
	GLfloat r = radius;
	GLfloat l = sqrtf(h*h + r*r);

	if (numberSlices < 3 || numberStacks < 1 || numberVertices > GL_MAX_VERTICES || numberIndices > GL_MAX_INDICES)
	{
		return GL_FALSE;
	}

	if (!shape)
	{
		return GL_FALSE;
	}
	ShapeInit(shape);

	shape->numberVertices = numberVertices;
	shape->numberIndices = numberIndices;

	shape->vertices = new glm::vec4[numberVertices];
	shape->normals = new glm::vec3[numberVertices];
	shape->tangents = new glm::vec3[numberVertices];
	shape->texCoords = new glm::vec2[numberVertices];
	shape->indices = new GLuint[numberIndices];

	if (!ShapeCheck(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	// Center bottom
	shape->vertices[vertexCounter].x = 0.0f;
	shape->vertices[vertexCounter].y = -halfExtend;
	shape->vertices[vertexCounter].z = 0.0f;
	shape->vertices[vertexCounter].w = 1.0f;

	shape->normals[vertexCounter].x = 0.0f;
	shape->normals[vertexCounter].y = -1.0f;
	shape->normals[vertexCounter].z = 0.0f;

	shape->tangents[vertexCounter].x = 0.0f;
	shape->tangents[vertexCounter].y = 0.0f;
	shape->tangents[vertexCounter].z = 1.0f;

	shape->texCoords[vertexCounter].x = 0.0f;
	shape->texCoords[vertexCounter].y = 0.0f;

	vertexCounter++;

	// Bottom
	for (i = 0; i < numberSlices + 1; i++)
	{
		GLfloat currentAngle = angleStep * (GLfloat)i;

		shape->vertices[vertexCounter].x = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter].y = -halfExtend;
		shape->vertices[vertexCounter].z = -sinf(currentAngle) * radius;
		shape->vertices[vertexCounter].w = 1.0f;

		shape->normals[vertexCounter].x = 0.0f;
		shape->normals[vertexCounter].y = -1.0f;
		shape->normals[vertexCounter].z = 0.0f;

		shape->tangents[vertexCounter].x = sinf(currentAngle);
		shape->tangents[vertexCounter].y = 0.0f;
		shape->tangents[vertexCounter].z = cosf(currentAngle);

		shape->texCoords[vertexCounter].x = 0.0f;
		shape->texCoords[vertexCounter].y = 0.0f;

		vertexCounter++;
	}

	for (j = 0; j < numberStacks + 1; j++)
	{
		GLfloat level = (GLfloat)j / (GLfloat)numberStacks;

		for (i = 0; i < numberSlices + 1; i++)
		{
			GLfloat currentAngle = angleStep * (GLfloat)i;

			shape->vertices[vertexCounter].x = cosf(currentAngle) * radius * (1.0f - level);
			shape->vertices[vertexCounter].y = -halfExtend + 2.0f * halfExtend * level;
			shape->vertices[vertexCounter].z = -sinf(currentAngle) * radius * (1.0f - level);
			shape->vertices[vertexCounter].w = 1.0f;

			shape->normals[vertexCounter].x = h / l * cosf(currentAngle);
			shape->normals[vertexCounter].y = r / l;
			shape->normals[vertexCounter].z = h / l * -sinf(currentAngle);

			shape->tangents[vertexCounter].x = -sinf(currentAngle);
			shape->tangents[vertexCounter].y = 0.0f;
			shape->tangents[vertexCounter].z = -cosf(currentAngle);

			shape->texCoords[vertexCounter].x = (GLfloat)i / (GLfloat)numberSlices;
			shape->texCoords[vertexCounter].y = level;

			vertexCounter++;
		}
	}

	indexIndices = 0;

	// Bottom
	centerIndex = 0;
	indexCounter = 1;

	for (i = 0; i < numberSlices; i++)
	{
		shape->indices[indexIndices++] = centerIndex;
		shape->indices[indexIndices++] = indexCounter + 1;
		shape->indices[indexIndices++] = indexCounter;

		indexCounter++;
	}
	indexCounter++;

	// Sides
	for (j = 0; j < numberStacks; j++)
	{
		for (i = 0; i < numberSlices; i++)
		{
			shape->indices[indexIndices++] = indexCounter;
			shape->indices[indexIndices++] = indexCounter + 1;
			shape->indices[indexIndices++] = indexCounter + numberSlices + 1;

			shape->indices[indexIndices++] = indexCounter + 1;
			shape->indices[indexIndices++] = indexCounter + numberSlices + 2;
			shape->indices[indexIndices++] = indexCounter + numberSlices + 1;

			indexCounter++;
		}
		indexCounter++;
	}

	if (!ShapeFinalize(shape))
	{
		ShapeDestroy(shape);

		return GL_FALSE;
	}

	return GL_TRUE;
}

GLboolean CalculateTangentBitangent(GLUSshape* shape)
{
	GLuint i;

	if (!shape || !shape->vertices || !shape->texCoords || shape->mode != GL_TRIANGLES)
	{
		return GL_FALSE;
	}

	// Allocate memory if needed
	if (!shape->tangents)
	{
		shape->tangents = new glm::vec3[shape->numberVertices];

		if (!shape->tangents)
		{
			return GL_FALSE;
		}
	}

	if (!shape->bitangents)
	{
		shape->bitangents = new glm::vec3[shape->numberVertices];

		if (!shape->bitangents)
		{
			return GL_FALSE;
		}
	}

	// Reset all tangents to 0.0f
	for (i = 0; i < shape->numberVertices; i++)
	{
		shape->tangents[i].x = 0.0f;
		shape->tangents[i].y = 0.0f;
		shape->tangents[i].z = 0.0f;

		shape->bitangents[i].x = 0.0f;
		shape->bitangents[i].y = 0.0f;
		shape->bitangents[i].z = 0.0f;
	}

	if (shape->numberIndices > 0)
	{
		float s1, t1, s2, t2;
		glm::vec4 Q1;
		glm::vec4 Q2;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		float scalar;

		for (i = 0; i < shape->numberIndices; i += 3)
		{
			s1 = shape->texCoords[shape->indices[i + 1]].x - shape->texCoords[shape->indices[i]].x;
			t1 = shape->texCoords[shape->indices[i + 1]].y - shape->texCoords[shape->indices[i]].y;
			s2 = shape->texCoords[shape->indices[i + 2]].x - shape->texCoords[shape->indices[i]].x;
			t2 = shape->texCoords[shape->indices[i + 2]].y - shape->texCoords[shape->indices[i]].y;

			scalar = 1.0f / (s1*t2 - s2*t1);

			Q1 = shape->vertices[shape->indices[i + 1]] - shape->vertices[shape->indices[i]];
			Q1.w = 1.0f;
			Q2 = shape->vertices[shape->indices[i + 2]] - shape->vertices[shape->indices[i]];
			Q2.w = 1.0f;

			tangent.x = scalar * (t2 * Q1.x - t1 * Q2.x);
			tangent.y = scalar * (t2 * Q1.y - t1 * Q2.y);
			tangent.z = scalar * (t2 * Q1.z - t1 * Q2.z);

			bitangent.x = scalar * (-s2 * Q1.x + s1 * Q2.x);
			bitangent.y = scalar * (-s2 * Q1.y + s1 * Q2.y);
			bitangent.z = scalar * (-s2 * Q1.z + s1 * Q2.z);

			tangent = glm::normalize(tangent);

			bitangent = glm::normalize(bitangent);

			shape->tangents[shape->indices[i]] += tangent;
			shape->tangents[shape->indices[i + 1]] += tangent;
			shape->tangents[shape->indices[i + 2]] += tangent;

			shape->bitangents[shape->indices[i]] += bitangent;
			shape->bitangents[shape->indices[i + 1]] += bitangent;
			shape->bitangents[shape->indices[i + 2]] += bitangent;
		}
	}
	else
	{
		float s1, t1, s2, t2;
		glm::vec4 Q1;
		glm::vec4 Q2;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		float scalar;

		for (i = 0; i < shape->numberVertices; i += 3)
		{
			s1 = shape->texCoords[(i + 1)].x - shape->texCoords[i].x;
			t1 = shape->texCoords[(i + 1)].y - shape->texCoords[i].y;
			s2 = shape->texCoords[(i + 2)].x - shape->texCoords[i].x;
			t2 = shape->texCoords[(i + 2)].y - shape->texCoords[i].y;

			scalar = 1.0f / (s1*t2 - s2*t1);

			Q1 = shape->vertices[(i + 1)] - shape->vertices[i];
			Q1.w = 1.0f;
			Q2 = shape->vertices[(i + 2)] - shape->vertices[i];
			Q2.w = 1.0f;

			tangent.x = scalar * (t2 * Q1.x - t1 * Q2.x);
			tangent.y = scalar * (t2 * Q1.y - t1 * Q2.y);
			tangent.z = scalar * (t2 * Q1.z - t1 * Q2.z);

			bitangent.x = scalar * (-s2 * Q1.x + s1 * Q2.x);
			bitangent.y = scalar * (-s2 * Q1.y + s1 * Q2.y);
			bitangent.z = scalar * (-s2 * Q1.z + s1 * Q2.z);

			tangent = glm::normalize(tangent);

			bitangent = glm::normalize(bitangent);

			shape->tangents[i] += tangent;
			shape->tangents[i + 1] += tangent;
			shape->tangents[i + 2] += tangent;

			shape->bitangents[i] += bitangent;
			shape->bitangents[i + 1] += bitangent;
			shape->bitangents[i + 2] += bitangent;
		}
	}

	// Normalize, as several triangles have added a vector
	for (i = 0; i < shape->numberVertices; i++)
	{
		shape->tangents[i] = glm::normalize(shape->tangents[i]);
		shape->bitangents[i] = glm::normalize(shape->bitangents[i]);
	}

	return GL_TRUE;
}

GLboolean  ShapeCopy(GLUSshape* shape, const GLUSshape* source)
{
	GLuint stride = 4 + 3 + 3 + 3 + 2;

	if (!shape || !source)
	{
		return GL_FALSE;
	}

	ShapeInit(shape);

	shape->numberVertices = source->numberVertices;
	shape->numberIndices = source->numberIndices;
	shape->mode = source->mode;

	if (source->vertices)
	{
		shape->vertices = new glm::vec4[source->numberVertices];
		if (!shape->vertices)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->vertices, source->vertices, 4 * source->numberVertices * sizeof(GLfloat));
	}
	if (source->normals)
	{
		shape->normals = new glm::vec3[source->numberVertices];
		if (!shape->normals)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->normals, source->normals, 3 * source->numberVertices * sizeof(GLfloat));
	}
	if (source->tangents)
	{
		shape->tangents = new glm::vec3[source->numberVertices];
		if (!shape->tangents)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->tangents, source->tangents, 3 * source->numberVertices * sizeof(GLfloat));
	}
	if (source->bitangents)
	{
		shape->bitangents = new glm::vec3[source->numberVertices];
		if (!shape->bitangents)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->bitangents, source->bitangents, 3 * source->numberVertices * sizeof(GLfloat));
	}
	if (source->texCoords)
	{
		shape->texCoords = new glm::vec2[source->numberVertices];
		if (!shape->texCoords)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->texCoords, source->texCoords, 2 * source->numberVertices * sizeof(GLfloat));
	}
	if (source->allAttributes)
	{
		shape->allAttributes = new GLfloat[source->numberVertices];
		if (!shape->allAttributes)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->allAttributes, source->allAttributes, stride * source->numberVertices * sizeof(GLfloat));
	}
	if (source->indices)
	{
		shape->indices = new GLuint[source->numberIndices];
		if (!shape->indices)
		{
			ShapeDestroy(shape);

			return GL_FALSE;
		}
		memcpy(shape->indices, source->indices, source->numberIndices * sizeof(GLuint));
	}

	return GL_TRUE;
}

void  ShapeDestroy(GLUSshape* shape)
{
	if (!shape)
	{
		return;
	}

	if (shape->vertices)
	{
		delete[] shape->vertices;

		shape->vertices = 0;
	}

	if (shape->normals)
	{
		delete[] shape->normals;

		shape->normals = 0;
	}

	if (shape->tangents)
	{
		delete[] shape->tangents;

		shape->tangents = 0;
	}

	if (shape->bitangents)
	{
		delete[] shape->bitangents;

		shape->bitangents = 0;
	}

	if (shape->texCoords)
	{
		delete[] shape->texCoords;

		shape->texCoords = 0;
	}

	if (shape->allAttributes)
	{
		delete[] shape->allAttributes;

		shape->allAttributes = 0;
	}

	if (shape->indices)
	{
		delete[] shape->indices;

		shape->indices = 0;
	}

	shape->numberVertices = 0;
	shape->numberIndices = 0;
	shape->mode = 0;
}
