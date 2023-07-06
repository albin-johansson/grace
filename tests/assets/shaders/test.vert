#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoords;

layout (std140, set = 0, binding = 0) uniform Matrices {
    mat4 uProjMatrix;
    mat4 uViewMatrix;
};

layout (std140, push_constant) uniform PushConstants {
    mat4 uModelMatrix;
};

void main()
{
    const mat4 mv_matrix = uViewMatrix * uModelMatrix;
    const mat4 mvp_matrix = uProjMatrix * mv_matrix;

    gl_Position = mvp_matrix * vec4(vPosition, 1);
}
