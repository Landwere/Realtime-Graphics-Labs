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

void main()
{
	vec3 lightDir = normalize(lightPosWorld - fragPosWorld);
	vec3 viewDir = normalize(cameraPos.xyz - fragPosWorld);
	float lightDistance = length(lightPosWorld - fragPosWorld);

	vec3 albedo = texture(albedoTex, texCoord).xyz;

	
	float specHighExponent = specularExponent * 2;
	vec3 diffuse = albedo.rgb;
	float specNorm = (specularIntensity + 8) / 8;
	float spec = ((pow( clamp( dot(normalize(lightDir + viewDir), worldNorm), 0, 1), specHighExponent ) * specNorm) * specularIntensity);
	

	colorOut.rgb = ((diffuse + spec) * lightIntensity / pow(lightDistance, falloffExponent)) * clamp( dot(lightDir, worldNorm), 0, 1);
	colorOut.a = 1;
}