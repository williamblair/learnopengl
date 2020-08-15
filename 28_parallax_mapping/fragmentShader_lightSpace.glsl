#version 330 core
in vec4 FragPos;

uniform vec3 uLightPos;
uniform float uFarPlane;

void main()
{
    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - uLightPos);

    // map to [0,1] range
    lightDistance = lightDistance / uFarPlane;

    // write this as the modified depth
    gl_FragDepth = lightDistance;
}

