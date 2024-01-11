
#version 410

in vec2 texCoords;

uniform vec3 particleColor;

out vec4 fragColor;

uniform vec3 camPosWorld;
in vec3 pointPos;

uniform int depthPass;
void main()
{

	float radialFalloff = 1 - 2*length(texCoords - vec2(0.5, 0.5));

	fragColor = vec4(particleColor, radialFalloff);

	
	
}

