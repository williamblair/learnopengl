#version 450 core

in vec4 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 fColor;

uniform sampler2D myTexture;
uniform sampler2D myTexture2;

uniform vec4 uLightColor;
uniform vec3 uLightPos;
uniform vec3 uViewPos;

// ambient lighting
float ambiance = 0.1;

// specular lighting
float specularStrength = 0.8;

void main()
{
    // just the texture
    //fColor = texture(myTexture, texCoord);

    // mix colors
    //fColor = texture(myTexture, texCoord) * color;

    // mix textures
    // 0.2 = percent of the second texture used (e.g. 80% first tex, 20% second tex)
    //fColor = mix(texture(myTexture, texCoord), texture(myTexture2, texCoord), 0.5);

    // diffuse
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(uLightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0); // no negative colors!
    vec3 diffuse = diff * vec3(uLightColor);
    
    // calculate specular component
    vec3 viewDir = normalize(uViewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); // could change power for more/less shininess!
    vec3 specular = specularStrength * spec * vec3(uLightColor);

    // combine
    vec3 result = vec3(vec4((ambiance + diffuse + specular), 1.0) * color);
    fColor = vec4(result, 1.0);


    // DEBUG!
    //fColor = vec4(normalize(normal), 1.0);
}

