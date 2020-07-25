#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uScreenTexture;

vec4 sharpenEffect()
{
    float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top left
        vec2(0.0, offset),      // top center
        vec2(offset, offset),   // top right
        vec2(-offset, 0.0),     // center left
        vec2(0.0, 0.0),         // center center
        vec2(offset, 0.0),      // center right
        vec2(-offset, -offset), // bottom left
        vec2(0.0, -offset),     // bottom center
        vec2(offset, -offset)   // bottom right
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    vec3 texSamples[9];
    for (int i = 0; i < 9; i++)
    {
        texSamples[i] = vec3(texture(uScreenTexture, TexCoords.st + offsets[i]));
    }
    vec3 colorRes = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        colorRes += texSamples[i] * kernel[i];
    }

    return vec4(colorRes, 1.0);
}

vec4 blurEffect()
{
    float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top left
        vec2(0.0, offset),      // top center
        vec2(offset, offset),   // top right
        vec2(-offset, 0.0),     // center left
        vec2(0.0, 0.0),         // center center
        vec2(offset, 0.0),      // center right
        vec2(-offset, -offset), // bottom left
        vec2(0.0, -offset),     // bottom center
        vec2(offset, -offset)   // bottom right
    );

    float kernel[9] = float[](
        1.0/16, 2.0/16, 1.0/16,
        2.0/16, 4.0/16, 2.0/16,
        1.0/16, 2.0/16, 1.0/16
    );

    vec3 texSamples[9];
    for (int i = 0; i < 9; i++)
    {
        texSamples[i] = vec3(texture(uScreenTexture, TexCoords.st + offsets[i]));
    }
    vec3 colorRes = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        colorRes += texSamples[i] * kernel[i];
    }

    return vec4(colorRes, 1.0);
}

vec4 edgeDetectionEffect()
{
    float offset = 1.0 / 300.0;
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top left
        vec2(0.0, offset),      // top center
        vec2(offset, offset),   // top right
        vec2(-offset, 0.0),     // center left
        vec2(0.0, 0.0),         // center center
        vec2(offset, 0.0),      // center right
        vec2(-offset, -offset), // bottom left
        vec2(0.0, -offset),     // bottom center
        vec2(offset, -offset)   // bottom right
    );

    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 texSamples[9];
    for (int i = 0; i < 9; i++)
    {
        texSamples[i] = vec3(texture(uScreenTexture, TexCoords.st + offsets[i]));
    }
    vec3 colorRes = vec3(0.0);
    for (int i = 0; i < 9; i++) {
        colorRes += texSamples[i] * kernel[i];
    }

    return vec4(colorRes, 1.0);
}

void main()
{
    // regular, unmodified screen
    //FragColor = texture(uScreenTexture, TexCoords);

    // inverted colors
    //FragColor = vec4(vec3(1.0 - texture(uScreenTexture, TexCoords)), 1.0);

    // grayscale (with weighted color components)
    //FragColor = texture(uScreenTexture, TexCoords);
    //float average = 0.2126*FragColor.r + 0.7152*FragColor.g + 0.0722*FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);

    // sharpening kernel (sample other texture coordinates with convolutional kernel)
    //FragColor = sharpenEffect();

    // blurring kernel (sample other texture coordinates with convolutional kernel)
    //FragColor = blurEffect();

    // edge detection kernel (sample other texture coordinates with convolutional kernel)
    FragColor = edgeDetectionEffect();
}

