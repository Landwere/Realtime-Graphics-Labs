
#version 410

in vec3 fragPosWorld;

uniform vec3 camPosWorld;

uniform vec4 color;
uniform float nearPlane, farPlane;
out vec4 colourOut;

void main()
{
	// Your code here
	// Save some depth values here so you can use them later for shadow mapping
	// Use the standard shader output value gl_FragDepth.
	// You'll probably want to scale your depths to be in the [0,1] range
	// based on the current nearPlane & farPlane values.
	float lightLength = distance(fragPosWorld, camPosWorld);
	if(lightLength > 15f || lightLength < 0f)
	{
		gl_FragDepth = 1;}
	else{
		gl_FragDepth = 0;}
	//gl_FragDepth = 0;
	//colourOut = vec4(0.0,0.0,0.0,0.0);
	//gl_FragDepth = clamp( (lightLength - nearPlane) / (farPlane - nearPlane) ,0, 1);
}

