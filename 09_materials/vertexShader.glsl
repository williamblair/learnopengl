#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// input transformation matrix from CPU/main application
uniform mat4 uTransform;

// model matrix for transforming the position to world space for lighting calc
uniform mat4 uModel;

// normal and fragment position (in world space) for lighting calculation
out vec3 Normal;
out vec3 FragPosition;

void main()
{
    gl_Position = uTransform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // this transpose/inverse is expensive and should techincally be
    // pre computed for efficiency
    // the matrix aNormal is multiplied by is called the normal matrix
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    FragPosition = vec3(uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0));
}
