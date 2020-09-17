#version 330 core
out vec4 FragColor;

in vec3 localPos;

uniform samplerCube uEnvMap;

void main()
{
    vec3 envColor = texture(uEnvMap, localPos).rgb;
    //FragColor = vec4(envColor.rgb, 1.0);

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    FragColor = vec4(envColor, 1.0);
}

