
#version 410

out vec4 colorOut;
uniform vec4 color;
in vec3 fragPosWorld;
in vec3 worldNorm;
in vec2 texCoord;

uniform samplerCube shadowMap;
uniform vec3 lightPosWorld;
uniform sampler2D albedoTex;

uniform float nearPlane, farPlane;

void main()
{
	// Your code here
	// Perform a shadow test here - compare the real distance from light to fragment
	// with the value you load from your depth texture.
	// Don't forget to scale your depth back to the right range using the farPlane
	// and nearPlane values.
	// If the point is in shadow, scale down the light intensity

	vec3 albedo = texture(albedoTex, texCoord).xyz;
	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
	vec4 diffuseColor = vec4( albedo.xyz * dot(worldNorm, lightDir), 1.0f);
	

	vec3 colorRgb = diffuseColor.rgb ;

	float realDist = distance(lightPosWorld, fragPosWorld);
	float lightDist = texture(shadowMap, -lightDir).r * (farPlane - nearPlane) + nearPlane;
	if( realDist > lightDist + 1.0f )
	{
		colorRgb *= 0.1f;
	}
	// if real distance > light distance add shadow

	colorOut.rgb = colorRgb;
	colorOut.a = 1.0;
}

