
#version 410

in vec3 fragPosWorld;

uniform vec3 lightPosWorld;

uniform vec4 color;
uniform float nearPlane, farPlane;

void main()
{

	//set frag depth scaled between 0 and 1
	float lightLength = length(fragPosWorld - lightPosWorld);
	gl_FragDepth = clamp( (lightLength - nearPlane) / (farPlane - nearPlane) ,0, 1);
}

