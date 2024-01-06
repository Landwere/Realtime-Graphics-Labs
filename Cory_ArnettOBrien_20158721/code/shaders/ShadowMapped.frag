
#version 410

out vec4 colorOut;
uniform vec4 color;
in vec3 fragPosWorld;
in vec3 worldNorm;
in vec2 texCoord;

uniform samplerCube shadowMap;
uniform vec3 lightPosWorld;
uniform sampler2D albedoTex;
uniform float bias;

uniform float nearPlane, farPlane;
uniform vec3 spotLightDir;

uniform vec3 worldLightDir;
uniform float worldLightInt;


vec3 lightModel(float lIntensity, vec3 lDirection, vec3 albedo, bool sCasting)
{
	vec3 diffuse = albedo.rgb;
	float realDist = distance(lightPosWorld, fragPosWorld);
	float lightDist = texture(shadowMap, -lDirection).r * (farPlane - nearPlane) + nearPlane;
	
	// if real distance > light distance and light is shadow casting add shadow
	if( realDist > lightDist + bias && sCasting)
	{
		diffuse *= 0.1f;
	}
	return (diffuse) * lIntensity * clamp( dot(lDirection, worldNorm), 0, 1);
}

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

	float spot = pow(max(dot(-lightDir, spotLightDir), 0.0f), 1);

	vec3 colorRgb = diffuseColor.rgb ;



	colorOut.rgb += (lightModel(2, lightDir, albedo.xyz, true) * spot );
	
	colorOut.rgb += (lightModel(worldLightInt, worldLightDir, albedo.xyz, false));

	colorOut.a = 1.0;
}

