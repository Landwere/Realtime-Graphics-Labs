#version 420

//shader from https://learnopengl.com/Advanced-Lighting/HDR
out vec4 colourOut;

in vec2 texCoord;

uniform sampler2D HDRBuffer;

void main()
{
	vec4 HDRColour = texture(HDRBuffer, texCoord);
	colourOut = vec4(HDRColour);
}

//end source