#version 330 core

uniform sampler2D diffuseMap;
uniform sampler2D emissiveMap;

uniform vec4 diffuseColor;
uniform vec4 emissiveColor;


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
	float spec = pow(max(0.0f, dot(half, normal)), shininess);
	float intensity = ambient * k_ambient + k_diffuse * NL + k_specular * spec;
	
	vec4 tex = texture2D(diffuseMap, UV0);
	vec4 emissiveTex = texture2D(emissiveMap, UV0);
	vec4 emissive = vec4(emissiveTex.r * emissiveColor.r, emissiveTex.g * emissiveColor.g, emissiveTex.b * emissiveColor.b, 1.0f);

	color = emissive + vec4(intensity * tex.r * diffuseColor.r, intensity * tex.g * diffuseColor.g, intensity * tex.b * diffuseColor.b, 1.0f);
	
}
