#version 410

in vec4 clipSpace;

uniform sampler2D reflectionTexture;

out vec4 colorOut;


void main()
{

	vec2 nDeviceSpace = (clipSpace.xy / clipSpace.w) / 2.0f + 0.5f;
	vec2 reflectTexCoords = vec2(nDeviceSpace.x, -nDeviceSpace.y);

	vec4 reflectionColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColour = vec4(0,0,1,0.1);
	colorOut = reflectionColour;
	colorOut = mix(reflectionColour, refractionColour, 0.5);
	//colorOut += vec4(0,0,0.1,1);
	//colorOut.rgb = vec3(0,0,1);
}