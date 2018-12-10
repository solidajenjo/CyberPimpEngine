#version 330 core

uniform vec4 object_color;
uniform sampler2D texture0;
uniform int useTex;

uniform vec3  light_pos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;

out vec4 color;

in vec2 UV0;

in vec3 normal;
in vec3 half;
in vec3 light_dir;
in vec3 eye_pos;

void main()
{
    float NL = max(0.0f, dot(normal, light_dir));
	float spec = pow(max(0.0f, dot(half, normal)), 111);
	float intensity = ambient * k_ambient + k_diffuse * NL + k_specular * spec;
	color = vec4(normal.x, normal.y, normal.z, 1.f);

	if(useTex == 1) 
	{
		vec4 tex = texture2D(texture0, UV0);
		color = vec4(intensity*tex.r, intensity*tex.g, intensity*tex.b, 1.0); 
	}
	else
		color = vec4(intensity*object_color.r, intensity*object_color.g, intensity*object_color.b, 1.0); 
}
