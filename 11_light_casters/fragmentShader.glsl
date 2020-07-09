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
    vec3 position; // for positional and spotlight
    vec3 direction; // for directional light and spotlight
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // positional light properties
    //float constant;
    //float linear;
    //float quadratic;

    // spotlight properties
    float cutoff;
    float outerCutoff; // for smoothing the edges of the light
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
    
    // for positional light
    //vec3 lightDir = normalize(uLight.position - FragPosition);
    //float lightDistance = length(uLight.position - FragPosition);
    //float attenuation = 1.0 / (uLight.constant + uLight.linear * lightDistance +
    //                           uLight.quadratic * (lightDistance * lightDistance));

    // for spotlight
    vec3 lightDir = normalize(uLight.position - FragPosition);
    float theta = dot(lightDir, normalize(-uLight.direction)); // remember dot product = cosine
    float epsilon = uLight.cutoff - uLight.outerCutoff;
    float intensity = clamp((theta - uLight.outerCutoff) / epsilon, 0.0, 1.0);

    // For directional light:
    // negate to specify the direction FROM the light source as opposed
    // to towards
    //vec3 lightDir = normalize(-uLight.direction);

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

    
    // for positional light
    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;

    // for spotlight
    // leave ambient unaffected so there's always a bit of light
    diffuse *= intensity;
    specular *= intensity;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
    //FragColor = vec4(lightDistance*0.05, lightDistance*0.05, lightDistance*0.05, 1.0);
    //FragColor = vec4(FragPosition, 1.0);
    //FragColor = vec4(TexCoords.x, TexCoords.y, 0.0, 1.0);
    //FragColor = vec4(vec3(texture(uMaterial.diffuse, TexCoords)), 1.0);
    //FragColor = vec4((ambient + diffuse + specular) * uObjectColor, 1.0F);
    //FragColor = vec4((ambient + diffuse) * uObjectColor, 1.0F);
    //FragColor = vec4(normalize(specular), 1.0F);
    //FragColor = vec4(uCameraPosition, 1.0);
    //FragColor = vec4(norm.xyz, 1.0);
}

