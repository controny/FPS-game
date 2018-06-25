#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadowMap;

uniform vec3 lightDirection;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform float shininess;

uniform int shadow_type;
uniform bool shadow_enable;

float random(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898, 78.233))) * 43758.5453);
}

float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) {
        // outside the view
        return 0.0;
    }

    float currentDepth = projCoords.z;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float shadow = 0.0;

    if (shadow_type == 0) {  // linear
        for(int x = -2; x <= 2; ++x) {
            for(int y = -2; y <= 2; ++y) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        return shadow / 25.0;
    }
    
    if (shadow_type != 0) {
        // blocker search
        int region = 0;
        if (shadow_type == 2) {  // PCSS 
            float light_width = 10.0f;
            region = max(int(light_width * (currentDepth - bias + closestDepth)), 4);
        } else {
            region = 2;  // fix blocker search region
        }

        float dist = 0;
        int blockers = 0;
        for(int x = -region; x <= region; ++x) {
            for(int y = -region; y <= region; ++y) {
                float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                if (currentDepth - bias > pcfDepth) {
                    blockers++;
                    dist += pcfDepth;
                }
            }
        }

        if (blockers == 0)
            return 0.0; // not shadow
        else {
            float blockerDistance = dist / blockers;

            // PCF width
            int penumbraWidth = min(max(int((currentDepth - blockerDistance) / blockerDistance * 2.0), 0), 10);
            //penumbraWidth = 2;

            for(int x = -penumbraWidth; x <= penumbraWidth; ++x) {
                for(int y = -penumbraWidth; y <= penumbraWidth; ++y) {
                    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
                    shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }

            shadow /= (2 * penumbraWidth + 1) * (2 * penumbraWidth + 1);
            return shadow;
        }
    }
}

void main()
{    
    vec3 color = texture(texture_diffuse1, TexCoords).rgb;

	// vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(Normal);
    // ambient
    vec3 ambient = ambientStrength * color;
    // diffuse
    vec3 lightDir = normalize(-lightDirection);
    vec3 diffuse =  max(dot(lightDir, normal), 0.0) * diffuseStrength * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    vec3 specular = pow(max(dot(normal, halfwayDir), 0.0), shininess) * specularStrength * lightColor * texture(texture_specular1, TexCoords).rgb;    
    
	// calculate shadow
	//float bias = max(0.25 * (1.0 - dot(normal, lightDir)), 0.025);
    float bias = max(0.01 * pow(1.0 - dot(normal, lightDir), 2), 0.001f);
    float shadow;
	vec3 lighting;

	if (shadow_enable) {
		shadow = ShadowCalculation(FragPosLightSpace, bias); 
		lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	} else {
		lighting = (ambient + diffuse + specular) * color;
	}
        

	FragColor = vec4(lighting, 1.0);
	//FragColor = vec4((Normal + 1) / 2, 1.0);
}