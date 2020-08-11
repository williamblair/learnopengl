#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

//out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    vs_out.FragPos = vec3(uModel * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(uModel))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);

    // tangent space TBN matrix
    vec3 T = normalize(vec3(uModel * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(uModel * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(uModel * vec4(aNormal, 0.0)));
    mat3 TBN = mat3(T,B,N);
    vs_out.TBN = TBN;
}

