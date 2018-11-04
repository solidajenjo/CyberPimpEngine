#version 330 core

out vec4 color;

in vec2 uv0;

uniform sampler2D texture0;
uniform float uvMultiplier;

void main()
{
	if (uvMultiplier > 1)
		color = texture2D(texture0, uv0 * uvMultiplier) * 25;
	else
		color = texture2D(texture0, uv0 * uvMultiplier);
}
