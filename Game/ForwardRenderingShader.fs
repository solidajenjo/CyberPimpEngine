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
	light_dir = normalize(light_pos-position);
	eye_pos = -(transpose(mat3(view)) * view[3].xyz);
	half = normalize(normalize(eye_pos - position) + light_dir);
    float NL = max(0.0f, dot(normal, light_dir));

	vec4 occlusionTex = texture2D(occlusionMap, UV0);		
	vec4 emissiveTex = texture2D(emissiveMap, UV0);
	vec4 diffuseTex = texture2D(diffuseMap, UV0);
	vec4 specularTex = texture2D(specularMap, UV0);

	float frag_shininess = min(shininess, mix(0, 128, specularTex.a));

	float spec = pow(max(0.0f, dot(half, normal)), frag_shininess);
	

	vec4 emissive = vec4(emissiveTex.r * emissiveColor.r, emissiveTex.g * emissiveColor.g, emissiveTex.b * emissiveColor.b, 1.0f);
	
	float intensity = ambient * k_ambient * occlusionTex.r + k_diffuse * NL;
	vec4 diffuse = diffuseTex * diffuseColor * intensity;

	vec4 specular = vec4(specularTex.r * specularColor.r * k_specular * spec, 
						 specularTex.g * specularColor.g * k_specular * spec,
						 specularTex.b * specularColor.b * k_specular * spec, 1.0f);

	color = emissive + diffuse + specular;	
	
}
