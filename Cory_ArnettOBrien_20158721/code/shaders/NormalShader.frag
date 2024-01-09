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
in vec3 biTan;
in vec3 _tan;
in mat3 TBNMatrix;
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

uniform vec3 colourOverride;

//Light model used to add light to model, with additional normal input for normal mapping
vec4 lightModel(float lIntensity, vec3 lDirection, vec3 viewDir, vec3 albedo, vec3 normWorld)
{
	//float specHighExponent = specularExponent * 2;
	vec4 diffuse = vec4(albedo.xyz * dot(worldNorm, lDirection), 1.0f);
	//float specNorm = (specularIntensity + 8) / 8;
	//float spec = ((pow( clamp( dot(normalize(lDirection + viewDir), normWorld), 0, 1), specHighExponent ) * specNorm) * specularIntensity);
	
	float specularPower = pow(clamp(dot(reflect(lDirection, normWorld), -viewDir), 0, 1), 10.0f);
	vec4 specularColor = vec4(specularPower * vec3(1,1,1), 1.0);

	return (diffuse + specularColor) * lIntensity;
}

void main()
{

	vec3 lightDir =  normalize(lightPosWorld - fragPosWorld);
	vec3 viewDir = normalize(cameraPos.xyz - fragPosWorld);
	float lightDistance = length(lightPosWorld - fragPosWorld);
	vec3 albedo = texture(albedoTex, texCoord).xyz;
	vec4 normal = texture(normalTex, texCoord);
	
	//Calculate colour based on code from normal lab passed through to lightModel
	vec3 normScaled = vec3((normal.xyz * 2 ) -1);
	float a = dot(normScaled, worldNorm);
	vec3 mapNormal; /*TBNMatrix * normScaled;*/ vec3(dot( normScaled,  _tan),dot( normScaled,  biTan), dot(normScaled, worldNorm));
	mapNormal = (dot( normScaled,  _tan),dot( normScaled,  biTan), normScaled.z * worldNorm);
	vec4 diffuseColor = vec4( albedo.xyz * dot(worldNorm, lightDir), 1.0f);
	float specularPower = pow(clamp(dot(reflect(lightDir, mapNormal), -viewDir), 0, 1), 10);
	vec4 specularColor = vec4(specularPower * vec3(1,1,1), 1.0);
			//colorOut = (30.0f) * (diffuseColor + specularColor) / (pow(lightDistance, 2));





//	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
//	vec3 viewDir = normalize(cameraPos.xyz - fragPosWorld);
//	float lightDistance =  length(lightPosWorld - fragPosWorld) ;
//
//	vec3 albedo = texture(albedoTex, texCoord).xyz;
//	vec4 normal = texture(normalTex, texCoord);
//
	float falloff = pow(lightDistance, 2);

	//spot controls spotlight from lighthouse
	float spot = pow(max(dot(-lightDir, spotLightDir), 0.0f), 1);

	//reset the colour just in case
	colorOut.rgb = vec3(0);
			//colorOut += ((30.0f) * (diffuseColor + specularColor) * spot) / (pow(lightDistance, 2));

	//apply lighting based on normal map and lighthouse spotlight (half lightIntensity because normal mapping seems to add more light)
	colorOut += clamp((lightModel(lightIntensity / 2, lightDir, viewDir, albedo, mapNormal) * spot) / falloff, 0.f,1.f);
//

//	vec3 mappedWorldNorm = vec3((normal.xyz * 2 ) -1);
//
//
//
//	//colorOut.rgb = vec3(0);
//	//colorOut.a = 1;
//	//Point  
	//colorOut = lightModel(0.0f, lightDir, vec3(0,0,0), vec3(0,0,0), vec3(0,0,0));
	//colorOut  += (lightModel(lightIntensity , lightDir, viewDir, albedo, mapNormal) * 0.5f)  / falloff;
//
//	//apply global light (half lightIntensity because normal mapping seems to add more light)
	colorOut += clamp(lightModel(worldLightInt / 2, worldLightDir, viewDir, albedo, mapNormal),0.f,1.f);

	if(colourOverride != vec3(0,0,0))
		colorOut.rgb *= colourOverride;
}

