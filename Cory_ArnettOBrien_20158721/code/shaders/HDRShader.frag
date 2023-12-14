#version 420

//shader inspired from https://learnopengl.com/Advanced-Lighting/HDR
out vec4 colourOut;

in vec2 texCoord;

uniform sampler2D HDRBuffer;
uniform bool enabled;
void main()
{
	vec4 HDRColour = texture(HDRBuffer, texCoord);
	//luminance map
	float luminance = (0.299 * HDRColour.x) + (0.587 * HDRColour.y) + (0.114 * HDRColour.b);

	const float gamme = 1.0;

	//reinhard tone map
	vec3 map = HDRColour.xyz / ( HDRColour.xyz + vec3(1.0));

	map.xyz = pow(map.xyz, vec3(1.0 / gamme));
	if(enabled)
		colourOut = vec4(map.xyz, 1.0);
	else
		colourOut = HDRColour;
}

//end source