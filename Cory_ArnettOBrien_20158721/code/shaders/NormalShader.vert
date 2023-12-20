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

out vec3 worldNorm;
out vec3 fragPosWorld;
out vec2 texCoord;
out vec3 biTan;
out vec3 _tan;

void main()
{

	vec4 clipPlane = vec4(0, -1.0f, 0, 5.0f);

		vec4 worldPos = modelToWorld * vec4(vPos, 1.0);
		//water needs to clip objects above a certain height to give the illusion the water only reflects a certain distance
		gl_ClipDistance[0] = dot(worldPos, clipPlane);

	gl_Position = worldToClip *  modelToWorld * vec4(vPos, 1.0);
	fragPosWorld = (modelToWorld * vec4(vPos, 1.0)).xyz;
	texCoord = vTex;
	worldNorm = normToWorld * vNorm;
	biTan = normToWorld * vBiTan;
	_tan = normToWorld * vTan;
}