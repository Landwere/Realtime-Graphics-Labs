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


uniform float clipDist;
uniform float clipDir;
 vec4 clipPlane;

out vec4 clipSpace;

out vec3 fragPosWorld;

void main()
{


		gl_Position = worldToClip *  modelToWorld * vec4(vPos, 1.0);
		clipSpace = worldToClip * modelToWorld * vec4(vPos, 1.0f);
		fragPosWorld = (modelToWorld * vec4(vPos, 1.0)).xyz;

}