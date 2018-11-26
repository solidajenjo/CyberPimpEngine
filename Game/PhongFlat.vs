#version 330 

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

uniform vec3  light_pos;
uniform float ambient;
uniform float shininess;
uniform float k_ambient;
uniform float k_diffuse;
uniform float k_specular;

out vec2 uv0;
out float intensity;

void main()
{
	vec3 position = (model*vec4(vertex_position, 1.0)).xyz;
    vec3 normal = (model*vec4(vertex_normal, 0.0)).xyz;
    gl_Position = proj*view*model*vec4(vertex_position, 1.0);
		
    vec3 light_dir = normalize(light_pos-position);
    float NL = max(0.0f, dot(normal, light_dir));

	intensity = ambient * k_ambient + k_diffuse * NL;
}
