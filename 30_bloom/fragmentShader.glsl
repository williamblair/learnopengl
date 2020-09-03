#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor; // we're rendering to 2 buffers here

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light
{
    vec3 Position;
    vec3 Color;
};

uniform Light lights[4];
uniform sampler2D uDiffuseTex;
uniform vec3 uViewPos;


void main()
{
    vec3 color = texture(uDiffuseTex, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);

    // ambient
    vec3 ambient = 0.0 * color; // no ambient here i guess...

    // lighting
    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(uViewPos - fs_in.FragPos);
    for (int i = 0; i < 4; i++)
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = lights[i].Color * diff * color;
        vec3 result = diffuse;

        // attenuation (quadratic because gamme correction later)
        float distance = length(fs_in.FragPos - lights[i].Position);
        result *= 1.0 / (distance * distance);

        lighting += result;
    }

    vec3 result = ambient + lighting;
    FragColor = vec4(result, 1.0); // regular result buffer

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        BrightColor = vec4(result, 1.0);
    }
    else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

