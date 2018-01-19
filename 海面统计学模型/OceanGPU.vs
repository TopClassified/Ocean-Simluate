#version 430 core

layout(binding = 0) uniform sampler2D u_displacementMap;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

in vec4 a_vertex;
in vec2 a_texCoord;

out vec2 v_texCoord;
out vec3 WorldPos;
out vec4 vReflectCoordinates;

void main(void)
{
	v_texCoord = a_texCoord;

	vec4 displacement = vec4(0.0, texture(u_displacementMap, a_texCoord).r, 0.0, 0.0);

	WorldPos = vec3(u_Model * (a_vertex + displacement));

	gl_Position = u_Projection * u_View * vec4(WorldPos, 1.0f);
	
	vReflectCoordinates = gl_Position;
	//vReflectCoordinates = u_Projection * u_View * u_Model * u_ReflectMatrix * (a_vertex + displacement);
}
