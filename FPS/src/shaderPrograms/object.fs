#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;  
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float specularStrength;
uniform float diffuseStrength;
uniform float shininess;

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
    //float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    //vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
	vec3 lighting = (ambient + diffuse + specular) * color;
    
    FragColor = vec4(lighting, 1.0);
}