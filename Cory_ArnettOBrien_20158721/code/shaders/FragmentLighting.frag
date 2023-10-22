#version 330 core



in vec2 out_uv;
in vec3 out_normal;
in vec3 fragPosition;

uniform sampler2D textureSampler;
uniform vec3 lightColour;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

//fog
in float fog_amount;
uniform vec3 fog_colour;


out vec4 fragmentColour;

void main()
{
	//ambient light
	float ambientStrength = 1.0f;
	vec3 ambient = ambientStrength * lightColour;

	//diffuse lighting 
	vec3 normal = normalize(out_normal);
	vec3 lightDirection = normalize(lightPosition - fragPosition);
	float diff = max(dot(normal, lightDirection), 0.0f);
	vec3 diffuse = diff * lightColour;

	//specular lighting
	float specularStrength = 1.0f;
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 reflectDirection = reflect(-lightDirection, normal);
	float spec = pow(max(dot(viewDir, reflectDirection), 0.0f), 64);
	vec3 specular = specularStrength * spec * lightColour;

	//texture 
	vec3 textureColour = texture(textureSampler, out_uv).xyz;

	//combining them together
	vec3 result = (ambient + diffuse) * textureColour + specular;
	
	//fragmentColour = vec4(result, 1.0f);
	fragmentColour = vec4(mix(result, fog_colour, fog_amount).xyz,  1.0f);
};

