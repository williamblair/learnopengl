#version 450 core

in vec4 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in vec3 lightPos;

out vec4 fColor;

// the texture is now used as a diffuse map in Material
//uniform sampler2D myTexture;
//uniform sampler2D myTexture2;

// since we're calculating in view space this is just 0,0,0
//uniform vec3 uViewPos;

// groups together phong properties
struct Material {
    vec3 ambient;
    sampler2D diffuse;
    //vec3 specular;
    sampler2D specular;
    float shininess;
};

uniform Material material;

// different phong proprties specifically for the light
struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;

void main()
{
    // ambient
    //vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0); // no negative colors!
    //vec3 diffuse = diff * material.diffuse * light.diffuse;
    vec3 diffuse = diff * vec3(texture(material.diffuse, texCoord)) * light.diffuse;
    
    // calculate specular component
    vec3 viewDir = normalize(-fragPos); // same as 0,0,0 - fragPos (view Pos is 0)
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = material.specular * spec * light.specular;
    vec3 specular = vec3(texture(material.specular, texCoord)) * spec * light.specular;

    // combine
    vec3 result = ambient + diffuse + specular;
    fColor = vec4(result, 1.0);


    // DEBUG!
    //fColor = vec4(normalize(normal), 1.0);
}

