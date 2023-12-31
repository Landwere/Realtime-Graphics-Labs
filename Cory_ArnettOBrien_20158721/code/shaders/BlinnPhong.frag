#version 410


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

uniform float falloffExponent;

uniform vec3 worldLightDir;
uniform float worldLightInt;

uniform vec3 spotLightDir;

vec3 lightModel(float lIntensity, vec3 lDirection, vec3 viewDir, vec3 albedo)
{
	float specHighExponent = specularExponent * 2;
	vec3 diffuse = albedo.rgb;
	float specNorm = (specularIntensity + 8) / 8;
	float spec = ((pow( clamp( dot(normalize(lDirection + viewDir), worldNorm), 0, 1), specHighExponent ) * specNorm) * specularIntensity);
	
	return (diffuse + spec) * lIntensity * clamp( dot(lDirection, worldNorm), 0, 1);
}

void main()
{



	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
	vec3 viewDir = normalize(cameraPos.xyz - fragPosWorld);
	float lightDistance =  length(lightPosWorld - fragPosWorld) ;

	vec4 albedo = texture(albedoTex, texCoord);
	
	float falloff = pow(lightDistance, 2);

//	//dot product betwwen lighthouse light direction and object light direction
//	float shape = dot(spotLightDir ,lightDir);
//	float add;
//	add = 0;
////	if (shape > 0.5)
////		add = 1;
//	add = mix(0,1,shape);
//	if (add < 0.5 || add > 2)
//		add = 0;
	
	//transparency
	if(albedo.a < 1.0f)
	{
		discard;
	}


	float spot = pow(max(dot(-lightDir, spotLightDir), 0.0f), 1);

	colorOut.rgb = vec3(0);
	colorOut.a = 1;
	//Point  
	colorOut.rgb += (lightModel(lightIntensity, lightDir, viewDir, albedo.xyz) * spot  )  / falloff;
	//global light
	colorOut.rgb += lightModel(worldLightInt, worldLightDir, viewDir, albedo.xyz);
}

