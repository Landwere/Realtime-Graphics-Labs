
#version 410

in vec3 fragPosWorld;

uniform vec3 camPosWorld;

uniform vec4 color;
uniform float nearPlane, farPlane;
out vec4 colourOut;

void main()
{
	//get distance and apply to depth texture 
	float fragDist = distance(fragPosWorld, camPosWorld);
	if(fragDist > 15.f || fragDist < 0.f)
	{
		gl_FragDepth = 1;}
	else
	{
		gl_FragDepth = 0;}
		
	//to make depth test work with transparency, depth buffer would need alpha colour data
	//could be received from HDR framebuffer

	//gl_FragDepth = 0;
	//colourOut = vec4(0.0,0.0,0.0,0.0);
	//gl_FragDepth = clamp( (lightLength - nearPlane) / (farPlane - nearPlane) ,0, 1);
}

