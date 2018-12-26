#version 330 core

uniform sampler2D diffuseMap;
uniform sampler2D emissiveMap;
uniform sampler2D occlusionMap;
uniform sampler2D specularMap;

uniform vec4 diffuseColor;
uniform vec4 emissiveColor;
uniform vec4 specularColor;


uniform vec3  light_pos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;
uniform mat4 view;

out vec4 color;

in vec2 UV0;
in vec3 normal;
in vec3 position;

vec3 half;
vec3 light_dir;
vec3 eye_pos;

void main()
{
	color = vec4(normal.r, normal.g, normal.b, 1.0f);
	
}
