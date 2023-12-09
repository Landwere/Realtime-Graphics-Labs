#version 420

//shader from https://learnopengl.com/Advanced-Lighting/HDR
out vec4 colourOut;

in vec2 texCoord;

uniform sampler2D HDRBuffer;

void main()
{
	vec3 HDRColour = texture(HDRBuffer, texCoord).xyz;
	colourOut = vec4(1,1,1,1);//vec4(HDRColour, 1.0);
}

//end source