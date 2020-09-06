#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uPositionTex;
uniform sampler2D uNormalTex;
uniform sampler2D uAlbedoTex;
uniform sampler2D uSsaoTex;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};
uniform Light uLight;

uniform bool uUseSSAO;

void main()
{
    // retreive geometry data
    vec3 FragPos = texture(uPositionTex, TexCoords).rgb;
    vec3 Normal = texture(uNormalTex, TexCoords).rgb;
    vec3 Diffuse = texture(uAlbedoTex, TexCoords).rgb;
    float AmbientOcclusion = texture(uSsaoTex, TexCoords).r;
    AmbientOcclusion = uUseSSAO ? AmbientOcclusion : 1.0;

    // then calculate the lighting as usual
    vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
    vec3 lighting = ambient;
    vec3 viewDir = normalize(-FragPos); // normally would be viewPos - FragPos, but in view space view pos is 0
    
    // diffuse
    vec3 lightDir = normalize(uLight.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * uLight.Color;

    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
    vec3 specular = uLight.Color * spec;

    // attenuation
    float distance = length(uLight.Position - FragPos);
    float attenuation = 1.0 / (1.0 + uLight.Linear * distance + uLight.Quadratic * distance * distance);
    
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    FragColor = vec4(lighting, 1.0);
    //FragColor = vec4(AmbientOcclusion, AmbientOcclusion, AmbientOcclusion, 1.0);
}

