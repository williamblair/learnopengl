#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D uPositionTex;
uniform sampler2D uNormalTex;
uniform sampler2D uNoiseTex;

uniform vec3 uSamples[64];

// parameters
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

const float screen_width = 800.0;
const float screen_height = 600.0;
const float noisetex_width = 4;
const float noisetex_height = 4;
const vec2 noiseScale = vec2(screen_width / noisetex_width, screen_height / noisetex_height);

uniform mat4 uProjection;

void main()
{
    // get input for SSAO
    vec3 fragPos = texture(uPositionTex, TexCoords).xyz;
    vec3 normal = normalize(texture(uNormalTex, TexCoords).rgb);
    vec3 randVec = normalize(texture(uNoiseTex, TexCoords * noiseScale).xyz);

    // create tangent to view space matrix
    vec3 tangent = normalize(randVec - normal * dot(randVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // calculate occlusion
    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; i++)
    {
        // get sample position
        vec3 sample = TBN * uSamples[i];
        sample = fragPos + sample * radius;

        // project sample position into screen space
        vec4 offset = vec4(sample, 1.0);
        offset = uProjection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0-1.0

        // get sample depth
        float sampleDepth = texture(uPositionTex, offset.xy).z; // get the depth value of the kernel sample

        // range check and accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);

    FragColor = occlusion;
}

