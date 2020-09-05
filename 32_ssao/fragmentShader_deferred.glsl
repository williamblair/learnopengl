#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uPositionTex;
uniform sampler2D uNormalTex;
uniform sampler2D uAlbedoSpecTex;

struct Light
{
    vec3 Position;
    vec3 Color;
};

const int NUM_LIGHTS = 32;
uniform Light lights[NUM_LIGHTS];
uniform vec3 uViewPos;

void main()
{
    // retrieve data from G-buffer
    vec3 FragPos = texture(uPositionTex, TexCoords).rgb;
    vec3 Normal = texture(uNormalTex, TexCoords).rgb;
    vec3 Albedo = texture(uAlbedoSpecTex, TexCoords).rgb;
    float Specular = texture(uAlbedoSpecTex, TexCoords).a;
    
    // then calculate lighting as usual
    vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(uViewPos - FragPos);
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo *
        lights[i].Color;
        lighting += diffuse;
    }
    FragColor = vec4(lighting, 1.0);
}

