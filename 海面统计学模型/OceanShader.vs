#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texture;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
	gl_Position = Projection * View * Model * vec4(vertex, 1.0f);
}