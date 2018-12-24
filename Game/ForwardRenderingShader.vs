﻿#version 400 

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3  light_pos;

out vec2 UV0;
out vec3 position;
out vec3 normal;

void main()
{
	vec3 position = (model*vec4(vertex_position, 1.0)).xyz;
	normal = normalize((model*vec4(vertex_normal, 0.0)).xyz);
    gl_Position = proj*view*model*vec4(vertex_position, 1.0);
		
   	UV0 = vertex_uv0;
}
