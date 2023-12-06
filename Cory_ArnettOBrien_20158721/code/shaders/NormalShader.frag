#version 420


layout(std140) uniform cameraBlock
{
	mat4 worldToClip;
	vec4 cameraPos;
	vec4 cameraDir;
};
in vec3 worldNorm;
in vec3 fragPosWorld;
in vec2 texCoord;

out vec4 colorOut;

uniform vec4 albedo;
uniform float specularExponent;
uniform float specularIntensity;

uniform int lightingMode;
uniform vec3 lightPosWorld;
uniform float lightIntensity;

uniform sampler2D albedoTex;
uniform sampler2D normalTex;

uniform float falloffExponent;

uniform vec3 worldLightDir;
uniform float worldLightInt;

uniform vec3 spotLightDir;

vec4 lightModel(float lIntensity, vec3 lDirection, vec3 viewDir, vec3 albedo, vec3 normWorld)
{
	//float specHighExponent = specularExponent * 2;
	vec4 diffuse = vec4(albedo.xyz * dot(normWorld, lDirection), 1.0f);
	//float specNorm = (specularIntensity + 8) / 8;
	//float spec = ((pow( clamp( dot(normalize(lDirection + viewDir), normWorld), 0, 1), specHighExponent ) * specNorm) * specularIntensity);
	
	float specularPower = pow(clamp(dot(reflect(lDirection, normWorld), -viewDir), 0, 1), 10);
	vec4 specularColor = vec4(specularPower * vec3(1,1,1), 1.0);

	return (diffuse + specularColor) * lIntensity;
}

void main()
{

	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
	vec3 viewDir = normalize(cameraPos.xyz - fragPosWorld);
	float lightDistance = length(lightPosWorld - fragPosWorld);
	vec4 albedo = texture(albedoTex, texCoord);
	vec4 normal = texture(normalTex, texCoord);

	// --- Your Code Here
	// Note that we're just using the mesh normal here (transformed to world space)
	// Change this to sample your normal map texture, and use it to adjust the normal.
	// Remember to rescale the normal from [0,1] to [-1,1]

	//vec4 normalColor = vec4((normal.xyz * 2) - 1);
	vec3 normWorld = vec3((normal.xyz * 2 ) -1);
	vec4 diffuseColor = vec4(albedo.xyz * dot(normWorld, lightDir), 1.0f);
	float specularPower = pow(clamp(dot(reflect(lightDir, normWorld), -viewDir), 0, 1), 10);
	vec4 specularColor = vec4(specularPower * vec3(1,1,1), 1.0);

	colorOut = lightIntensity * (diffuseColor + specularColor) / (lightDistance*lightDistance);




//	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
//	vec3 viewDir = normalize(cameraPos.xyz - fragPosWorld);
//	float lightDistance =  length(lightPosWorld - fragPosWorld) ;
//
//	vec3 albedo = texture(albedoTex, texCoord).xyz;
//	vec4 normal = texture(normalTex, texCoord);
//
//	float falloff = pow(lightDistance, 2);
//
//
////	//dot product betwwen lighthouse light direction and object light direction
////	float shape = dot(spotLightDir ,lightDir);
////	float add;
////	add = 0;
//////	if (shape > 0.5)
//////		add = 1;
////	add = mix(0,1,shape);
////	if (add < 0.5 || add > 2)
////		add = 0;
//	
//	float spot = pow(max(dot(-lightDir, spotLightDir), 0.0f), 1);
//
//	vec3 mappedWorldNorm = vec3((normal.xyz * 2 ) -1);
//
//
//
//	//colorOut.rgb = vec3(0);
//	//colorOut.a = 1;
//	//Point  
//
//	colorOut = (lightModel(lightIntensity, lightDir, viewDir, albedo, mappedWorldNorm))  / falloff;
//
//	//global light
//	//colorOut.rgb += lightModel(worldLightInt, worldLightDir, viewDir, albedo, worldNorm);
}

