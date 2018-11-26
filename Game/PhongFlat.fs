#version 330 core

uniform vec4 object_color;

out vec4 color;

in float intensity;
in vec3 normal;

void main()
{
	//color = vec4(intensity*object_color.x, intensity*object_color.y, intensity*object_color.z, 1.0); 
	color = vec4(normal.x, normal.y, normal.z, 1.0); 
}
