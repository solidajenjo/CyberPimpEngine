#version 330 core

layout (location = 0) out vec4 color;
layout (location = 1) out vec4 position;
layout (location = 2) out vec4 normal;
layout (location = 3) out vec4 specularColor;
layout (location = 4) out vec4 diffuseColor;

struct DirectionalLight
{
	vec3 position;
	vec3 color;
};

struct PointLight
{
	vec3 position;
	vec3 color;
	vec3 attenuation; // 0 - Constant | 1 - Linear | 2 - Quadric
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	vec3 color;
	float inner;
	float outter;
	vec3 attenuation; // 0 - Constant | 1 - Linear | 2 - Quadric
};

struct Fog
{
	vec3 fogColor;
	float fogQuadratic;
	float fogFalloff;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gSpecular;

//lighting
uniform float appScale;
uniform int nPoints;
uniform PointLight lightPoints[100];
uniform int nSpots;
uniform SpotLight lightSpots[100];
uniform int nDirectionals;
uniform DirectionalLight lightDirectionals[2];
uniform Fog fogParameters;

uniform mat4 view;

in vec2 UV0;

float shininess;


vec3 lambert(vec3 light_dir)
{
	
    float NL = max(0.0f, dot(normal.xyz, light_dir));		
	return diffuseColor.rgb * NL;	
}

vec3 specular(vec3 half)
{
	float spec = pow(max(0.0f, dot(half, normal.xyz)), shininess);
	return specularColor.rgb * spec;
}

vec3 directionalBlinn(vec3 light_color, vec3 light_dir, vec3 eye_pos)
{
	vec3 half = normalize(normalize(eye_pos - position.xyz) + light_dir);

	vec3 lambertColor = lambert(light_dir);
	vec3 specColor = specular(half);

	return (lambertColor + specColor) * light_color;
}


vec3 pointBlinn(PointLight light, vec3 light_dir, vec3 eye_pos)
{	
	float distance = length(light_dir);	
	light_dir = -(light_dir / distance);
	vec3 half = normalize(normalize(eye_pos - position.xyz) + light_dir);

	float att = 1.0f / (light.attenuation[0]  + light.attenuation[1] * distance + light.attenuation[2] * (distance * distance));
	vec3 lambertColor = att * lambert(light_dir);
	vec3 specColor = att * specular(half);
	return (lambertColor + specColor) * light.color;	
}

vec3 spotBlinn(SpotLight light, vec3 light_dir, vec3 eye_pos)
{	
	float distance = length(light_dir);
	light_dir = light_dir / distance;
	vec3 half = normalize(normalize(eye_pos - position.xyz) + light_dir);

	vec3 light_direction = normalize(light.direction);
	float cos_a = min(1.0f, max(0.0f, (dot(light_dir, light.direction)-light.outter)/max(0.0001f, light.inner-light.outter)));
	float att = cos_a / (light.attenuation[0]  + light.attenuation[1] * distance + light.attenuation[2] * (distance * distance));
	vec3 lambertColor = att * lambert(light_dir);
	vec3 specColor = att * specular(half);
	return (lambertColor + specColor) * light.color;
}

void main()
{

	position = texture2D(gPosition, UV0);
	normal = texture2D(gNormal, UV0);
	diffuseColor = texture2D(gDiffuse , UV0);
	specularColor = texture2D(gSpecular, UV0);
	shininess = normal.a * 255;
	
	vec3 eye_pos = -(transpose(mat3(view)) * view[3].xyz);	//TODO:This on vertex shader?

	vec4 emissive = vec4(texture2D(gPosition, UV0).a ,texture2D(gDiffuse , UV0).a, texture2D(gSpecular, UV0).a, 1.f);

	color = vec4(0.f, 0.f, 0.f, 1.f); 

	for (int i = 0; i < nDirectionals; ++i)
	{
		vec3 light_dir = normalize(lightDirectionals[i].position);
		color = color + vec4(directionalBlinn(lightDirectionals[i].color, light_dir, eye_pos), 1.f);	
	}

	for (int i = 0; i < nPoints; ++i)
	{
		vec3 light_dir = position.xyz - lightPoints[i].position;
		color = color + vec4(pointBlinn(lightPoints[i], light_dir, eye_pos), 1.f);	
	}

	for (int i = 0; i < nSpots; ++i)
	{
		vec3 light_dir = position.xyz - lightSpots[i].position;
		color = color + vec4(spotBlinn(lightSpots[i], light_dir, eye_pos), 1.f);	
	}
	//color = vec4(shininess, shininess, shininess, 1);
	color = color + emissive;
	position.a = 1.f;
	normal.a = 1.f;
	diffuseColor.a = 1.f;
	specularColor.a = 1.f;
	//color = i;
//	color = vec4(i,i,i,1);

}
