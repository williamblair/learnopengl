#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalMap; // aka bump map
uniform sampler2D uDepthMap;

uniform float uHeightScale;

// calculates/offsets texture coordinates based on view dir
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(uDepthMap, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * uHeightScale);
    return texCoords - p;
}


void main()
{
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = ParallaxMapping(fs_in.TexCoords, viewDir);
    if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) {
        discard;
    }

    vec3 normal = texture(uNormalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 color = texture(uDiffuseTex, texCoords).rgb;
    vec3 ambient = 0.1 * color;
    
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

