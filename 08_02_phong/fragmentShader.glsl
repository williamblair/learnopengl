#version 330 core
out vec4 FragColor;
in vec3 FragPosition;
in vec3 Normal;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;
uniform vec3 uCameraPosition;

void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPosition - FragPosition);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    // 32 is shininess value of the hilight
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
    vec3 specular = specularStrength * spec * uLightColor;

    FragColor = vec4((ambient + diffuse + specular) * uObjectColor, 1.0F);
    //FragColor = vec4((ambient + diffuse) * uObjectColor, 1.0F);
    //FragColor = vec4(normalize(specular), 1.0F);
    //FragColor = vec4(uCameraPosition, 1.0);
    //FragColor = vec4(norm.xyz, 1.0);
}

