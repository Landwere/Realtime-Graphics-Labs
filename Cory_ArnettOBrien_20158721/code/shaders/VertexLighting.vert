#version 330 core

layout(location = 0) in vec4 vertexPosition;
layout(location = 1) in vec2 uvCoordinates;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

out vec2 out_uv;
out vec3 out_normal;
out vec3 fragPosition;


//fog
out float fog_amount;
uniform float fog_start;
uniform float fog_range;

void main()
{
	vec4 v = vec4(vertexPosition.xyz, 1);
	vec4 posInWorld = transform * v;
	v = projection * view * posInWorld;

	gl_Position = projection * view * transform * vertexPosition;
	out_uv = uvCoordinates;
	out_normal = vec3(transform * vec4(vertexNormal, 1.0f));
	fragPosition = vec3(transform * vertexPosition);

	vec4 pos_rel_eye = view * posInWorld;
	float distance = length(pos_rel_eye.xyz);
	fog_amount = (distance - fog_start) / fog_range;
	fog_amount = clamp(fog_amount, 0.0f, 1.0f);
};