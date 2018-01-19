#version 330 core
layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 Reflection;
uniform int Reflect;

void main()
{
	vec4 pos;
	if (Reflect == 1)
	{
		pos = projection * view * Reflection * vec4(position, 1.0);
	}
    else
	{
		pos = projection * view * vec4(position, 1.0);
	}
    gl_Position = pos.xyww;
    TexCoords = position;
}