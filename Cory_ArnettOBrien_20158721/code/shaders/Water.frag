#version 410

in vec4 clipSpace;
in vec3 fragPosWorld;

uniform sampler2D reflectionTexture;

out vec4 colorOut;
uniform vec3 spotLightDir;
uniform vec3 lightPosWorld;

void main()
{

	vec2 nDeviceSpace = (clipSpace.xy / clipSpace.w) / 2.0f + 0.5f;
	vec2 reflectTexCoords = vec2(nDeviceSpace.x, -nDeviceSpace.y);

	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
	float spot = pow(max(dot(-lightDir, spotLightDir), 0.0f), 1);


	vec4 reflectionColour = texture(reflectionTexture, reflectTexCoords);
	vec4 refractionColour = vec4(0,0,0.2f,0.1);
	colorOut = reflectionColour;
	colorOut = mix(reflectionColour, refractionColour, 0.5) * spot;
	//colorOut += vec4(0,0,0.1,1);
	//colorOut.rgb = vec3(0,0,1);
}