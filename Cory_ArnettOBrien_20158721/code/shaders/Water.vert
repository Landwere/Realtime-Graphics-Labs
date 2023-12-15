#version 410


layout(location = 0) in vec3 vPos;
layout(location = 2) in vec2 vTex;

layout(std140) uniform cameraBlock
{
	mat4 worldToClip;
	vec4 cameraPos;
	vec4 cameraDir;
};
uniform mat4 modelToWorld;

const vec4 clipPlane = vec4(0, -1, 0, 0.00000001);

out vec4 clipSpace;


void main()
{
		vec4 worldPos = modelToWorld * vec4(vPos, 1.0);
		gl_ClipDistance[0] = dot(worldPos, clipPlane);

		gl_Position = worldToClip *  modelToWorld * vec4(vPos, 1.0);
		clipSpace = worldToClip * modelToWorld * vec4(vPos, 1.0f);

}