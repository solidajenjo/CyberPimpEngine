#version 330 core

out vec4 color;

in vec2 uv0;

uniform sampler2D texture0;
uniform int useColor;
uniform vec4 colorU;

void main()
{
	if (useColor == 1)
		color = colorU;
	else
		color = texture2D(texture0, uv0) * colorU;
	//color = vec4(1,1,1,1);
}
