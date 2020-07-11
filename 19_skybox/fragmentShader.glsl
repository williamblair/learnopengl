#version 330 core
out vec4 FragColor;
in vec3 FragPosition;
in vec3 Normal;
in vec2 TexCoords;

// Phong lighting components
struct Material {
    //sampler2D diffuse; // diffuse map
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2; // 3 arbitrarily chosen for now idk
    sampler2D texture_diffuse3;
    //sampler2D specular;
    sampler2D texture_specular1;
    sampler2D texture_specular2; // also 3 arbitrarily chosen for now
    sampler2D texture_specular3;
    float shininess;
};

// Material instance
uniform Material uMaterial;

// Light source components
struct DirectionalLight {
    vec3 direction; // for directional light and spotlight
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct PositionalLight {
    vec3 position; // for positional and spotlight
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // positional light properties
    float constant;
    float linear;
    float quadratic;
};
struct SpotLight {
    vec3 position; // for positional and spotlight
    vec3 direction; // for directional light and spotlight

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // spotlight properties
    float cutoff;
    float outerCutoff; // for smoothing the edges of the light
};

// Light source instances
uniform DirectionalLight uDirLight;
uniform PositionalLight uPosLight;
uniform SpotLight uSpotLight;

uniform vec3 uCameraPosition;

vec3 calcDirectionalLight()
{
    vec3 norm = normalize(Normal);

    // For directional light:
    // negate to specify the direction FROM the light source as opposed
    // to towards
    vec3 lightDir = normalize(-uDirLight.direction);

    // ambient
    vec3 ambient =  uDirLight.ambient * vec3(texture(uMaterial.texture_diffuse1, 
                                                  TexCoords));

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uDirLight.diffuse * diff * vec3(texture(uMaterial.texture_diffuse1,
                                                         TexCoords));

    // specular
    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uDirLight.specular * vec3(texture(uMaterial.texture_specular1,
                                                    TexCoords))
                                              * spec;

    return (ambient + diffuse + specular);
}

vec3 calcPositionalLight()
{
    // ambient
    vec3 ambient =  uPosLight.ambient * vec3(texture(uMaterial.texture_diffuse1, 
                                                  TexCoords));

    vec3 norm = normalize(Normal);

    // for positional light
    vec3 lightDir = normalize(uPosLight.position - FragPosition);
    float lightDistance = length(uPosLight.position - FragPosition);
    float attenuation = 1.0 / (uPosLight.constant + uPosLight.linear * lightDistance +
                               uPosLight.quadratic * (lightDistance * lightDistance));

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uPosLight.diffuse * diff * vec3(texture(uMaterial.texture_diffuse1,
                                                         TexCoords));

    // specular
    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uPosLight.specular * vec3(texture(uMaterial.texture_specular1,
                                                    TexCoords))
                                        * spec;

    
    // for positional light
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 calcSpotLight()
{
    // ambient
    vec3 ambient =  uSpotLight.ambient * vec3(texture(uMaterial.texture_diffuse1, 
                                                  TexCoords));

    vec3 norm = normalize(Normal);

    // for spotlight
    vec3 lightDir = normalize(uSpotLight.position - FragPosition);
    float theta = dot(lightDir, normalize(-uSpotLight.direction)); // remember dot product = cosine
    float epsilon = uSpotLight.cutoff - uSpotLight.outerCutoff;
    float intensity = clamp((theta - uSpotLight.outerCutoff) / epsilon, 0.0, 1.0);

    // diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uSpotLight.diffuse * diff * vec3(texture(uMaterial.texture_diffuse1,
                                                         TexCoords));

    // specular
    vec3 viewDir = normalize(uCameraPosition - FragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shininess);
    vec3 specular = uSpotLight.specular * vec3(texture(uMaterial.texture_specular1,
                                                    TexCoords))
                                        * spec;

    // for spotlight
    // leave ambient unaffected so there's always a bit of light
    diffuse *= intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // convert to NDC: from [0.0,1.0] to [-1.0, 1.0]
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    vec3 dirLightColor = calcDirectionalLight();
    vec3 posLightColor = calcPositionalLight();
    vec3 spotLightColor = calcSpotLight();

    FragColor = vec4(dirLightColor + posLightColor + spotLightColor, 1.0);
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.0); // non-linear, default depth val
    //float linDepth = LinearizeDepth(gl_FragCoord.z) / far;
    //FragColor = vec4(vec3(linDepth), 1.0);

    //FragColor = vec4(ambient + diffuse + specular, 1.0);
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

