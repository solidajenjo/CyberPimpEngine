#version 330 core

out vec4 color;


uniform vec3 colorAxis;

void main()
{
    color = vec4(colorAxis, 1);
}
