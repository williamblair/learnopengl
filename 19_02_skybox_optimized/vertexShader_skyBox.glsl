#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords; // 3d instead of 2d because its for a cube map

uniform mat4 uProjection;
uniform mat4 uView;

void main()
{
    TexCoords = aPos;
    gl_Position = uProjection * uView * vec4(aPos, 1.0);
    gl_Position = gl_Position.xyww; // replace the z component with w component so perspective division
                                    // results in w/w = 1.0 = maximum depth, so the skybox is always
                                    // the furthest in the background
}

