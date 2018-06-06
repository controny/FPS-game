#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform float shininess;
uniform float bias;

float random(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0) {
        // outside the view
        return 0.0;
    }

    float currentDepth = projCoords.z;
    
	float closestDepth = texture(shadowMap, projCoords.xy).r; 

	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x) {
		for(int y = -1; y <= 1; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;

    shadow = 0.0;
	vec2 poissonDisk[4] = vec2[](vec2(-0.94201624, -0.39906216), vec2( 0.94558609, -0.76890725 ), vec2( -0.094184101, -0.92938870 ), vec2( 0.34495938, 0.29387760 ));
    for (int i = 0; i < 4; i++){
        int index = int(16.0 * random(vec2(gl_FragCoord.x, i))) % 16;
        if (texture(shadowMap, projCoords.xy + poissonDisk[i] / 700.0 ).r  <  currentDepth - bias)
            shadow += 0.25;
    }
    return shadow;
}

void main()
{    
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

	// vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(Normal);
    // ambient
    vec3 ambient = ambientStrength * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 diffuse =  max(dot(lightDir, normal), 0.0) * diffuseStrength * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), shininess) * lightColor * specularStrength;    
    
	// calculate shadow
	//float bias = max(0.25 * (1.0 - dot(normal, lightDir)), 0.025);
    float bias = max(0.01 * pow(1.0 - dot(normal, lightDir), 32), 0.001f);
    float shadow = ShadowCalculation(FragPosLightSpace, bias);                  
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

	FragColor = vec4(lighting, 1.0);
}