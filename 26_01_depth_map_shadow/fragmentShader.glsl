#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D uDiffuseTex;
uniform sampler2D uShadowMapTex;

uniform vec3 uLightPos;
uniform vec3 uViewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective division
    // since we're using orthographic projection, techinically this doesn't do 
    // anything currently
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // convert from [-1,1] to [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // get the calculated nearest and current fragment depth
    float closestDepth = texture(uShadowMapTex, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // set wether the fragment is a shadow or not if its depth
    // is farther back than the closest depth
    float isShadow = (currentDepth > closestDepth) ? 1.0 : 0.0;

    return isShadow;
}

void main()
{
    vec3 color = texture(uDiffuseTex, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);

    vec3 ambient = 0.15 * color;

    vec3 lightDir = normalize(uLightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(uViewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0); // 64 is shiny exponent
    vec3 specular = spec * lightColor;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}

