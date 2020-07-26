#version 330 core
out vec4 FragColor;
in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoords;
uniform vec3 uCameraPosition;

// Phong lighting components
struct Material {
    sampler2D texture_diffuse1;
};

// Material instance
uniform Material uMaterial;

// Light source components
struct PositionalLight {
    vec3 position; // for positional and spotlight
};

// Light source instances
uniform PositionalLight uPosLight;

uniform bool uBlinnShading;
uniform bool uGammaCorrection;
uniform float uGammaVal;

void main()
{
    vec3 color = texture(uMaterial.texture_diffuse1, TexCoords).rgb;

    vec3 ambient = 0.01 * color;

    vec3 lightDir = normalize(uPosLight.position - FragPosition);
    vec3 normal = normalize(Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    float spec = 0.0;
    if (uBlinnShading)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0); // 32 = power exponent
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(normal, reflectDir), 0.0), 32.0);
    }
    vec3 specular = vec3(0.3) * spec; // assuming bright white light color

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(abs(normal), 1.0);
    //FragColor = vec4(vec3(uBlinnShading), 1.0);
    //FragColor = vec4(texture(uMaterial.texture_diffuse1, TexCoords).rgb, 1.0);

    if (uGammaCorrection) {
        FragColor.rgb = pow(FragColor.rgb, vec3(1.0/uGammaVal));
    }
}

