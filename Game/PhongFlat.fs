#version 330 core

uniform vec4 object_color;
uniform sampler2D texture0;
uniform int useTex;

out vec4 color;

in float intensity;
in vec3 normal;
in vec2 UV0;

void main()
{
	if(useTex == 1) 
	{
		vec4 tex = texture2D(texture0, UV0);
		color = vec4(intensity*tex.r, intensity*tex.g, intensity*tex.b, 1.0); 
	}
	else
		color = vec4(intensity*object_color.r, intensity*object_color.g, intensity*object_color.b, 1.0); 
		//color = vec4(UV0.x, UV0.y, 0, 1.0); 
	
}
