#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec; // albedo (color) and specular

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light
{
    vec3 Position;
    vec3 Color;
};

//uniform Light lights[4];
uniform sampler2D uDiffuseTex;
uniform sampler2D uSpecularTex;
//uniform vec3 uViewPos;


void main()
{
    gPosition = fs_in.FragPos;
    gNormal = normalize(fs_in.Normal);
    gAlbedoSpec.rgb = texture(uDiffuseTex, fs_in.TexCoords).rgb;
    // TODO
    gAlbedoSpec.a = texture(uSpecularTex, fs_in.TexCoords).r;
    //gAlbedoSpec.a = 1.0;
}

