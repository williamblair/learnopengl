#version 330 core
out vec4 FragColor;
in vec3 FragPosition;
in vec3 Normal;

// Phong lighting components
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// Material instance
uniform Material uMaterial;

// Light source components
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Light source instance
uniform Light uLight;

uniform vec3 uLightPosition;
uniform vec3 uCameraPosition;

void main()
{
    // ambient
    vec3 ambient =  uLight.ambient * uMaterial.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPosition - FragPosition);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLight.diffuse * (diff * uMaterial.diffuse) ;

    // specular
    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uLight.specular * (uMaterial.specular * spec);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4((ambient + diffuse + specular) * uObjectColor, 1.0F);
    //FragColor = vec4((ambient + diffuse) * uObjectColor, 1.0F);
    //FragColor = vec4(normalize(specular), 1.0F);
    //FragColor = vec4(uCameraPosition, 1.0);
    //FragColor = vec4(norm.xyz, 1.0);
}

