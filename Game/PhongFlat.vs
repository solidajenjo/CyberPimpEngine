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

out vec2 UV0;

out vec3 normal;
out vec3 half;
out vec3 light_dir;
out vec3 eye_pos;

void main()
{
	vec3 position = (model*vec4(vertex_position, 1.0)).xyz;
    normal = normalize((model*vec4(vertex_normal, 0.0)).xyz);
    gl_Position = proj*view*model*vec4(vertex_position, 1.0);
		
    light_dir = normalize(light_pos-position);
	eye_pos = -(transpose(mat3(view)) * view[3].xyz);
	half = normalize(normalize(eye_pos - position) + light_dir);

	UV0 = vertex_uv0;
}
