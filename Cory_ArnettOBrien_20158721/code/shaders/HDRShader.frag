#version 420

//shader inspired from https://learnopengl.com/Advanced-Lighting/HDR
out vec4 colourOut;

in vec2 texCoord[9];

uniform sampler2D HDRBuffer;
uniform sampler2D duplicateBuffer;
uniform sampler2D depthTexture;
uniform bool enabled;

const float PI = 3.141;

vec4 blurPixel(vec2 texCoord)
{
	vec4 blurColour = texture(HDRBuffer, texCoord);

	float depth = texture(depthTexture, texCoord).r;
	if (depth < 0.1)
		return blurColour;
	float x = 0.4;
// tent function
if(x > 0.5)
    x = 1.0 - x; // from 0 to 0.5

x *= 2.0; // from 0 to 1
x *= 2.0; // from 0 to 2
x -= 1.0; // from -1 to 1

x = abs(x);
x = pow(x, 10.0);

	float directions = 16;
	float quality = 10;
	float radius = 2;
	for( float d=0.0; d<PI*2; d+= PI*2/directions)
        for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
            blurColour += texture( HDRBuffer, texCoord + vec2(cos(d),sin(d))*radius*x*i);
	
	blurColour /= quality * directions;
	return blurColour;
}

void main()
{
	vec4 HDRColour = texture(HDRBuffer, texCoord[0]);

	vec4 blur = (texture(HDRBuffer, texCoord[0]) + texture(HDRBuffer, texCoord[1]) + texture(HDRBuffer, texCoord[2]) + texture(HDRBuffer, texCoord[3]) + texture(HDRBuffer, texCoord[4]) 
	+ texture(HDRBuffer, texCoord[5]) + texture(HDRBuffer, texCoord[6]) + texture(HDRBuffer, texCoord[7]) + texture(HDRBuffer, texCoord[8]) / 9);

	//luminance map
	float luminance = (0.299 * HDRColour.x) + (0.587 * HDRColour.y) + (0.114 * HDRColour.b);

	const float gamme = 1.0;

	//reinhard tone map
	vec3 map = HDRColour.xyz / ( HDRColour.xyz + vec3(1.0));

	map.xyz = pow(map.xyz, vec3(1.0 / gamme));
	if(enabled)
		colourOut = vec4(map.xyz, 1.0);
	else
		colourOut = blurPixel(texCoord[0]);
}

//end source