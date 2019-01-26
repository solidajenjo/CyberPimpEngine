#version 400 

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;
layout(location = 3) in vec3 vertex_tangent;
layout(location = 4) in vec3 vertex_bitangent;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3  light_pos;

out vec2 UV0;
out vec3 position;
out vec3 normalIn;
out vec3 tan;
out vec3 bitan;

void main()
{
	position = (model*vec4(vertex_position, 1.0)).xyz;
	normalIn = mat3(transpose(inverse(model))) * vertex_normal;
	tan = (model * vec4(vertex_tangent, 1.0)).xyz;
	bitan = (model * vec4(vertex_bitangent, 1.0)).xyz;
    gl_Position = proj*view*model*vec4(vertex_position, 1.0);
   	UV0 = vertex_uv0;
}
