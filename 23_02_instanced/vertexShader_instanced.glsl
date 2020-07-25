#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
// the max size of an attribute is 4 floats, so
// this matrix will use attribute locations 3,4,5, and 6
layout (location = 3) in mat4 aInstanceMatrix;

// input transformation matrix from CPU/main application
uniform mat4 uProjection;
uniform mat4 uView;

// model matrix for transforming the position to world space for lighting calc
//uniform mat4 uModel;

// normal and fragment position (in world space) for lighting calculation
out vec3 Normal;
out vec3 FragPosition;
out vec2 TexCoords;

void main()
{
    gl_Position = uProjection * uView * aInstanceMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // this transpose/inverse is expensive and should techincally be
    // pre computed for efficiency
    // the matrix aNormal is multiplied by is called the normal matrix
    Normal = mat3(transpose(inverse(aInstanceMatrix))) * aNormal;
    FragPosition = vec3(aInstanceMatrix * vec4(aPos.x, aPos.y, aPos.z, 1.0));
    TexCoords = aTexCoords;
}
