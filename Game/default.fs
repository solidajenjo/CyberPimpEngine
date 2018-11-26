#version 330 core

out vec4 color;

in vec2 uv0;

uniform sampler2D texture0;
uniform int useColor;
uniform vec4 colorU;

void main()
{
	color = vec4(0.5, 0.5, 0.2, 1.0);
	
}
