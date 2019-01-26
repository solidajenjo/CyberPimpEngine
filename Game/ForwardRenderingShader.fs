#version 400 core

struct Material
{
	sampler2D diffuseMap;
	sampler2D emissiveMap;
	sampler2D occlusionMap;
	sampler2D specularMap;
	sampler2D normalMap;
	vec4 diffuseColor;
	vec4 emissiveColor;
	vec4 specularColor;	
	float shininess;
	float k_ambient;
	float k_diffuse;
	float k_specular;
};

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

uniform Material mat;

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

layout (location = 0) out vec4 color;

in vec2 UV0;
in vec3 normalIn;
in vec3 position;
in vec3 tan;
in vec3 bitan;
in mat4 Mmodel;

vec3 normal;

//uniform subroutines

subroutine vec3 NormalSubroutine(); // Signature
subroutine uniform NormalSubroutine getNormal; // pointer

subroutine (NormalSubroutine) vec3 useNormalMap()
{
	vec3 normalM = texture(mat.normalMap, UV0).xyz;
	normalM = normalize(normalM * 2.0 - 1.0);

	vec3 T = normalize(tan);
	vec3 B = normalize(bitan);
	vec3 N = normalize(normalIn);	
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * normalM);
}
subroutine (NormalSubroutine) vec3 modelNormal()
{
	return normalIn;
}


//lighting computation

vec4 lambert(vec3 light_dir, vec4 occlusionTex, vec4 diffuseTex)
{
	
    float NL = max(0.0f, dot(normal, light_dir));	
	float intensity = (mat.k_ambient + mat.k_diffuse) * NL;
	return diffuseTex * mat.diffuseColor * intensity * occlusionTex.r;	
}

vec4 specular(vec3 half, vec4 specularTex)
{
	float frag_shininess = min(mat.shininess, mix(0, 128, specularTex.a));
	float spec = pow(max(0.0f, dot(half, normal)), frag_shininess);
	return vec4(specularTex.r * mat.specularColor.r * mat.k_specular * spec, 
						 specularTex.g * mat.specularColor.g * mat.k_specular * spec,
						 specularTex.b * mat.specularColor.b * mat.k_specular * spec, .0f);
}

vec4 directionalBlinn(vec4 light_color, vec3 light_dir, vec3 eye_pos, vec4 occlusionTex, vec4 diffuseTex, vec4 specularTex)
{
	vec3 half = normalize(normalize(eye_pos - position) + light_dir);

	vec4 lambertColor = lambert(light_dir, occlusionTex, diffuseTex);
	vec4 specColor = specular(half, specularTex);
	return (lambertColor + specColor) * light_color;
}

vec4 pointBlinn(PointLight light, vec3 light_dir, vec3 eye_pos, vec4 occlusionTex, vec4 diffuseTex, vec4 specularTex)
{	
	float distance = length(light_dir);
	light_dir = -(light_dir / distance);
	vec3 half = normalize(normalize(eye_pos - position) + light_dir);

	float att = 1.0f / (light.attenuation[0]  + light.attenuation[1] * distance + light.attenuation[2] * (distance * distance));
	vec4 lambertColor = att * lambert(light_dir, occlusionTex, diffuseTex);
	vec4 specColor = att * specular(half, specularTex);
	return (lambertColor + specColor) * vec4(light.color, 1.0f);	
}

vec4 spotBlinn(SpotLight light, vec3 light_dir, vec3 eye_pos, vec4 occlusionTex, vec4 diffuseTex, vec4 specularTex)
{	
	float distance = length(light_dir);
	light_dir = light_dir / distance;
	vec3 half = normalize(normalize(eye_pos - position) + light_dir);

	float cos_a = min(1.0, max(0.0, (dot(light_dir, light.direction) - light.outter)/max(0.0001, light.inner - light.outter)));
	float att = cos_a / (light.attenuation[0]  + light.attenuation[1] * distance + light.attenuation[2] * (distance * distance));
	vec4 lambertColor = att * lambert(-light_dir, occlusionTex, diffuseTex);
	vec4 specColor = att * specular(half, specularTex);
	return (lambertColor + specColor) * vec4(light.color, 1.0f);	
}

//main program

void main()
{

	normal = getNormal();

	vec4 occlusionTex = texture2D(mat.occlusionMap, UV0);		
	vec4 emissiveTex = texture2D(mat.emissiveMap, UV0);
	vec4 diffuseTex = texture2D(mat.diffuseMap, UV0);
	vec4 specularTex = texture2D(mat.specularMap, UV0);

	vec3 eye_pos = -(transpose(mat3(view)) * view[3].xyz);	//TODO:This on vertex shader?

	color = vec4(emissiveTex.r * mat.emissiveColor.r, emissiveTex.g * mat.emissiveColor.g, emissiveTex.b * mat.emissiveColor.b, min(mat.diffuseColor.a, diffuseTex.a));
	for (int i = 0; i < nDirectionals; ++i)
	{
		vec3 light_dir = normalize(lightDirectionals[i].position);
		color = color + directionalBlinn(vec4(lightDirectionals[i].color, 1.f), light_dir, eye_pos, occlusionTex, diffuseTex, specularTex);	
	}

	for (int i = 0; i < nPoints; ++i)
	{
		vec3 light_dir = position - lightPoints[i].position;
		color = color + pointBlinn(lightPoints[i], light_dir, eye_pos, occlusionTex, diffuseTex, specularTex);	
	}

	for (int i = 0; i < nSpots; ++i)
	{
		vec3 light_dir = position - lightSpots[i].position;
		color = color + spotBlinn(lightSpots[i], light_dir, eye_pos, occlusionTex, diffuseTex, specularTex);	
	}

	float fragDistance = length(position - eye_pos);
	float fogAmount = fogParameters.fogFalloff * fragDistance + fogParameters.fogFalloff * fogParameters.fogQuadratic * fragDistance * fragDistance;	

	color = color + vec4(vec3(fogAmount, fogAmount, fogAmount) * fogParameters.fogColor, 0.f);	
	
}
