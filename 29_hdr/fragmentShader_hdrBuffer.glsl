#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uHdrBuffer;
uniform bool uHdr;
uniform float uExposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(uHdrBuffer, TexCoords).rgb;

    if (uHdr)
    {
        // reinhard HDR calculation
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));

        // exposure HDR calculation
        vec3 result = vec3(1.0) - exp(-hdrColor * uExposure);

        // gamma correction
        result = pow(result, vec3(1.0 / gamma));

        FragColor = vec4(result, 1.0);
        //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        // just gamma correction
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        FragColor = vec4(result, 1.0);
    }
}

