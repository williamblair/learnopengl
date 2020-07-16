#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 GeomNormal[];
in vec3 GeomFragPosition[];
in vec2 GeomTexCoords[];
out vec3 Normal;        // to forward to the fragment shader
out vec3 FragPosition;
out vec2 TexCoords;

uniform float uTime;

vec3 calcNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a,b));
}

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    // convert from -1,1 to 0,1.0 then multiply by normal and magnitude
    vec3 direction = normal * ((sin(uTime) + 1.0) / 2.0) * magnitude;

    return position + vec4(direction, 0.0);
}

void main()
{
    vec3 explodeNormal = calcNormal();

    gl_Position = explode(gl_in[0].gl_Position, explodeNormal);
    Normal = GeomNormal[0];
    FragPosition = GeomFragPosition[0];
    TexCoords = GeomTexCoords[0];
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, explodeNormal);
    Normal = GeomNormal[1];
    FragPosition = GeomFragPosition[1];
    TexCoords = GeomTexCoords[1];
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, explodeNormal);
    Normal = GeomNormal[2];
    FragPosition = GeomFragPosition[2];
    TexCoords = GeomTexCoords[2];
    EmitVertex();

    EndPrimitive();
}

