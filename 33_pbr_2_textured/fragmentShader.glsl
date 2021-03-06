#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// Untextured
//uniform vec3 uAlbedo;
//uniform float uMetallic;
//uniform float uRoughness;
//uniform float uAo;
uniform sampler2D uAlbedoTex;
uniform sampler2D uMetallicTex;
uniform sampler2D uRoughnessTex;
uniform sampler2D uAoTex;
uniform sampler2D uNormalTex;

uniform vec3 uLightPositions[4];
uniform vec3 uLightColors[4];

uniform vec3 uCamPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float numerator = a2;
    float denominator = NdotH2 * (a2 - 1.0) + 1.0;
    denominator = PI * denominator * denominator;

    return (numerator / denominator);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    
    float numerator = NdotV;
    float denominator = NdotV * (1.0 - k) + k;

    return numerator / denominator;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx2 * ggx1;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(uNormalTex, TexCoords).xyz * 2.0 - 1.0;

    // partial derivative of an arg with respect to x or y
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/dFdx.xhtml
    vec3 Q1 = dFdx(WorldPos);
    vec3 Q2 = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B = -normalize(cross(N,T));
    mat3 TBN = mat3(T,B,N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    // convert HDR (sRGB) to linear space because albedo texture normally is in sRGB space
    vec3 albedo = pow(texture(uAlbedoTex, TexCoords).rgb, vec3(2.2));
    vec3 normal = getNormalFromMap();
    float metallic = texture(uMetallicTex, TexCoords).r;
    float roughness = texture(uRoughnessTex, TexCoords).r;
    float ao = texture(uAoTex, TexCoords).r;

    vec3 N = normalize(normal);
    vec3 V = normalize(uCamPos - WorldPos);

    // use 0.04 if the material is non-metallic  (metallic is 1.0 or 0.0 or in between)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation (4 = number of lights)
    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 4; i++)
    {
        // calculate per light radiance
        vec3 L = normalize(uLightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(uLightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = uLightColors[i] * attenuation;
        
        // cook-torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlick(max(dot(H,V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic; // make kD = 0 if metallic is 1.0 (metallic surfaces do not refract, so no diffuse reflections)

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);

        // add outgoing radiance to Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // Gamma correction (required for PBR)
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);
}

