#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCords;

uniform mat4 matWorldInverseT;
uniform mat4 matWorld;

uniform mat4 projection;

out vec3 pos; 
out vec3 norm;

void main()
{
	pos = position;
	norm = normal;
	gl_Position = projection * vec4(position, 1.0);
	
	
} 
