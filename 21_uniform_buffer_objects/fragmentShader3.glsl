#version 330 core
out vec4 FragColor;
//in vec2 texCoord;

//uniform sampler2D texSampler1;
//uniform sampler2D texSampler2;

void main()
{
    FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    //FragColor = vertexColor;
    //FragColor = texture(texSampler, texCoord);
    //FragColor = texture(texSampler1, texCoord) * vertexColor;

    // 0.2 = 20% of second color, 80% of first color
    //FragColor = mix(texture(texSampler1, texCoord),
    //                texture(texSampler2, texCoord), 0.2) * vertexColor;
    //FragColor = mix(texture(texSampler1, texCoord),
    //                texture(texSampler2, texCoord), 0.2);
}

