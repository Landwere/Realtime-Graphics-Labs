#version 410

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec2 vTex;
layout(location = 4) in vec3 vTan;
layout(location = 5) in vec3 vBiTan;

layout(std140) uniform cameraBlock
{
	mat4 worldToClip;
	vec4 cameraPos;
	vec4 cameraDir;
};
uniform mat4 modelToWorld;
uniform mat3 normToWorld;

out vec3 normWorld;
out vec3 fragPosWorld;
out vec2 texCoord;
out vec3 biTan;
out vec3 _tan;

void main()
{
	gl_Position = worldToClip *  modelToWorld * vec4(vPos, 1.0);
	fragPosWorld = (modelToWorld * vec4(vPos, 1.0)).xyz;
	texCoord = vTex;
	normWorld = normToWorld * vNorm;
	biTan = normToWorld * vBiTan;
	_tan = normToWorld * vTan;
}