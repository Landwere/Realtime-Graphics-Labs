#version 420

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 vTex;

out vec2 texCoord[9];

void main()
{
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
	texCoord[0] = vTex;
	texCoord[1] = vTex + vec2(1,0);
	texCoord[2] = vTex + vec2(-1,0);
	texCoord[3] = vTex + vec2(0,1);
	texCoord[4] = vTex + vec2(0,-1);
	texCoord[5] = vTex + vec2(1,1);
	texCoord[6] = vTex + vec2(1,-1);
	texCoord[7] = vTex + vec2(-1,1);
	texCoord[8] = vTex + vec2(-1,-1);
}