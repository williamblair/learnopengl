#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 GeomNormal[];
in vec3 GeomFragPosition[];
in vec2 GeomTexCoords[];
out vec3 Normal;        // to forward to the fragment shader
out vec3 FragPosition;
out vec2 TexCoords;

void main()
{
    gl_Position = gl_in[0].gl_Position;
    Normal = GeomNormal[0];
    FragPosition = GeomFragPosition[0];
    TexCoords = GeomTexCoords[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    Normal = GeomNormal[1];
    FragPosition = GeomFragPosition[1];
    TexCoords = GeomTexCoords[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    Normal = GeomNormal[2];
    FragPosition = GeomFragPosition[2];
    TexCoords = GeomTexCoords[2];
    EmitVertex();

    EndPrimitive();
}

