#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

// vertex shader output
in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

uniform mat4 uProjection;

void generateLine(int index)
{
    gl_Position = uProjection * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = uProjection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    generateLine(0);
    generateLine(1);
    generateLine(2);
}

