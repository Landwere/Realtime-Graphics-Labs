
#version 410

layout(location = 0) in vec3 vPos;

layout(std140) uniform cameraBlock
{
	mat4 worldToClip;
	vec4 cameraPos;
	vec4 cameraDir;
};
uniform mat4 modelToWorld;

out vec3 fragPosWorld;
out vec2 texCoord;

void main()
{
	fragPosWorld = (modelToWorld * vec4(vPos, 1.0)).xyz;
	gl_Position = worldToClip *  modelToWorld * vec4(vPos, 1.0);
}

