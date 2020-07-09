#version 330 core
out vec4 FragColor;
in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoords;

// Phong lighting components
struct Material {
    sampler2D diffuse; // diffuse map
    sampler2D specular;
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

uniform vec3 uCameraPosition;

void main()
{
    // ambient
    vec3 ambient =  uLight.ambient * vec3(texture(uMaterial.diffuse, 
                                                  TexCoords));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLight.position - FragPosition);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uLight.diffuse * diff * vec3(texture(uMaterial.diffuse,
                                                         TexCoords));

    // specular
    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uLight.specular * vec3(texture(uMaterial.specular,
                                                    TexCoords))
                                        * spec;

    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(TexCoords.x, TexCoords.y, 0.0, 1.0);
    //FragColor = vec4(vec3(texture(uMaterial.diffuse, TexCoords)), 1.0);
    //FragColor = vec4((ambient + diffuse + specular) * uObjectColor, 1.0F);
    //FragColor = vec4((ambient + diffuse) * uObjectColor, 1.0F);
    //FragColor = vec4(normalize(specular), 1.0F);
    //FragColor = vec4(uCameraPosition, 1.0);
    //FragColor = vec4(norm.xyz, 1.0);
}

