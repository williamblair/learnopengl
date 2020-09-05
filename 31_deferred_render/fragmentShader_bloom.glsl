#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uScene; // regulary rendered scene (no effects)
uniform sampler2D uBloomBlur; // the blurred texture
uniform bool uBloom;
uniform float uExposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(uScene, TexCoords).rgb;
    vec3 bloomColor = texture(uBloomBlur, TexCoords).rgb;

    if (uBloom)
    {
        hdrColor += bloomColor; // additive blending
    }

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * uExposure);

    // gamma correction
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}

