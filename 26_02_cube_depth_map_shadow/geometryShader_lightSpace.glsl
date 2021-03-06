#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 uLightTransforms[6];

// output per emitvertex
out vec4 FragPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable: which face to render
        for (int i = 0; i < 3; i++) // each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = uLightTransforms[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

