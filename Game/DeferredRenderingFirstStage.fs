#version 330 core
layout (location = 1) out vec4 gPosition;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gSpecular;
layout (location = 0) out vec4 gDiffuse;

struct Material
{
	sampler2D diffuseMap;
	sampler2D emissiveMap;
	sampler2D occlusionMap;
	sampler2D specularMap;
	vec4 diffuseColor;
	vec4 emissiveColor;
	vec4 specularColor;	
	float shininess;
	float k_ambient;
	float k_diffuse;
	float k_specular;
};

struct Fog
{
	vec3 fogColor;
	float fogQuadratic;
	float fogFalloff;
};

uniform Fog fogParameters;

in vec2 UV0;
in vec3 normal;
in vec3 position;


uniform Material mat;
uniform mat4 view;

void main()
{    
	vec4 occlusionTex = texture2D(mat.occlusionMap, UV0);
	vec4 specularTex = texture2D(mat.specularMap, UV0);
	vec4 emissiveTex = texture2D(mat.emissiveMap, UV0);
    
	gPosition = vec4(position, 1.f);
    gNormal =vec4(normal, min(mat.shininess, mix(0, 128, specularTex.a)) / 255);
	vec3 eye_pos = -(transpose(mat3(view)) * view[3].xyz);	//TODO:This on vertex shader?
    
	gDiffuse.rgb = texture(mat.diffuseMap, UV0).rgb * mat.diffuseColor.rgb * (mat.k_ambient + mat.k_diffuse) * occlusionTex.r; 
	
	float fragDistance = length(position - eye_pos);
		
	float fogAmount = fogParameters.fogFalloff * fragDistance + fogParameters.fogFalloff * fogParameters.fogQuadratic * fragDistance * fragDistance;	

	gDiffuse = gDiffuse + vec4(fogAmount, fogAmount, fogAmount, 1.f) * vec4(fogParameters.fogColor, 1.f);
	
	gSpecular = texture2D(mat.specularMap, UV0) * mat.specularColor * mat.k_specular;
	vec3 emmisive = mat.emissiveColor.rgb * emissiveTex.rgb;

	gPosition.a = emmisive.r;
	gDiffuse.a = emmisive.g;
	gSpecular.a = emmisive.b;

	////
}  