#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

uniform sampler2D uDiffuseTex;
uniform sampler2D uNormalMap; // aka bump map
uniform samplerCube uShadowMapTex;

uniform vec3 uLightPos;
uniform vec3 uViewPos;

uniform float uFarPlane;

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - uLightPos;
    float closestDepth = texture(uShadowMapTex, fragToLight).r;

    // transform from [0,1] to [0,farplane]
    closestDepth *= uFarPlane;

    float currentDepth = length(fragToLight);

// original, no PCF filtering
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

// pcf filtering (percentage closer filtering)
// this method uses 64 samples, which is alot
//  a more efficient method would be to define an array of sample offsets (e.g. 1,1,1,  1,-1,1,  etc.)
//  and iterate through those
/*
    float shadow = 0.0;
    float bias = 0.05;
    float samples = 4.0;
    float offset = 0.1;
    for (float x = -offset; x < offset; x += offset / (samples * 0.5))
    {
        for (float y = -offset; y < offset; y += offset / (samples * 0.5))
        {
            for (float z = -offset; z < offset; z += offset / (samples * 0.5))
            {
                float closestDepth = texture(uShadowMapTex, fragToLight + vec3(x,y,z)).r;
                closestDepth *= uFarPlane;
                if (currentDepth - bias > closestDepth) {
                    shadow += 1.0;
                }
            }
        }
    }
    shadow /= (samples * samples * samples); // x,y,z at 4 each = 64
*/

    // DEBUG!!!
    //FragColor = vec4(vec3(closestDepth/uFarPlane), 1.0);
    //FragColor = vec4(vec3(fragToLight), 1.0);
    ////////////

    return shadow;
}

void main()
{
    vec3 color = texture(uDiffuseTex, fs_in.TexCoords).rgb;

// original normal, stored in the vertices
    vec3 normal = normalize(fs_in.Normal);

// normal map without TBN space calculation
    //vec3 normal = texture(uNormalMap, fs_in.TexCoords).rgb;
    //normal = normalize(normal * 2.0 - 1.0); // map from [0,1] to [-1,1]
    // DEBUG!!!!!!!!!!!!!!!!!!
    //FragColor = vec4(normal, 1.0);
    //////////////////////////

// normal map with TBN space calculation
    //vec3 normal = texture(uNormalMap, fs_in.TexCoords).rgb;
    //normal = normal * 2.0 - 1.0; // convert from [0,1] to [-1,1]
    //normal = normalize(fs_in.TBN * normal);

    vec3 lightColor = vec3(0.8);

    // ambient
    vec3 ambient = 0.05 * color;

    // diffuse
    vec3 lightDir = normalize(uLightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(uViewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPos);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}

