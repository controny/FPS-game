#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D heightMap;

uniform samplerCube irradianceMap;

//uniform bool use_height_map;

// shadow
uniform sampler2D shadowMap;
uniform int shadow_type;
uniform bool shadow_enable;

// lights
// uniform vec3 lightDirection;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 viewPos;

const float PI = 3.14159265359;
const float height_scale = 0.05;

// ----------------------------------------------------------------------------
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
// ----------------------------------------------------------------------------
vec2 ParallaxMapping()
{
    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec3 viewDir = normalize(TBN * viewPos - TBN * FragPos);

    // // number of depth layers
    // const float minLayers = 10;
    // const float maxLayers = 20;
    // float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // // calculate the size of each layer
    // float layerDepth = 1.0 / numLayers;
    // // depth of current layer
    // float currentLayerDepth = 0.0;
    // // the amount to shift the texture coordinates per layer (from vector P)
    // vec2 P = viewDir.xy / viewDir.z * height_scale; 
    // vec2 deltaTexCoords = P / numLayers;
  
    // // get initial values
    // vec2  currentTexCoords     = TexCoords;
    // float currentDepthMapValue = texture(heightMap, currentTexCoords).r;
      
    // while(currentLayerDepth < currentDepthMapValue)
    // {
    //     // shift texture coordinates along direction of P
    //     currentTexCoords -= deltaTexCoords;
    //     // get depthmap value at current texture coordinates
    //     currentDepthMapValue = texture(heightMap, currentTexCoords).r;  
    //     // get depth of next layer
    //     currentLayerDepth += layerDepth;  
    // }
    
    // // -- parallax occlusion mapping interpolation from here on
    // // get texture coordinates before collision (reverse operations)
    // vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // // get depth after and before collision for linear interpolation
    // float afterDepth  = currentDepthMapValue - currentLayerDepth;
    // float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // // interpolation of texture coordinates
    // float weight = afterDepth / (afterDepth - beforeDepth);
    // vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    // return finalTexCoords;
    float height =  texture(heightMap, TexCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
    return TexCoords - p;    
}
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragPos);
    vec3 Q2  = dFdy(FragPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{		
    // vec2 texCoords;
    // if (use_height_map) {
    //     texCoords = ParallaxMapping();
    // } else {
    //     texCoords = TexCoords;
    // }
    // vec2 texCoords = ParallaxMapping();
    vec2 texCoords = TexCoords;
    vec3 albedo     = pow(texture(albedoMap, texCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, texCoords).r;
    float roughness = texture(roughnessMap, texCoords).r;
    float ao        = texture(aoMap, texCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(viewPos - FragPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // calculate per-light radiance
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (distance * distance);
    // vec3 radiance = lightColor * attenuation;
    vec3 radiance = lightColor;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 nominator    = NDF * G * F; 
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
    vec3 specular = nominator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);        

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    
    // ambient lighting
    vec3 ambient = vec3(0.1) * albedo * ao;
    // vec3 ambient = vec3(0.5f);
    
    vec3 color;
    if (shadow_enable) {
        float bias = max(0.01 * pow(1.0 - dot(N, L), 2), 0.001f);
        float shadow = ShadowCalculation(FragPosLightSpace, bias); 
        color = ambient + (1.0 - shadow) * Lo;
    } else {
        color = ambient + Lo;
    }

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}
