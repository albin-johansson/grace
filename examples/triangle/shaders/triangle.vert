#version 450 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;

layout (location = 0) out VsOutput {
    vec3 color;
} Out;

layout (std140, set = 0, binding = 0) uniform Matrices {
    mat4 uProjectionMatrix;
    mat4 uViewMatrix;
};

layout (std140, push_constant) uniform PushConstants {
    mat4 uModelMatrix;
};

void main()
{
    const mat4 mvp_matrix = uProjectionMatrix * uViewMatrix * uModelMatrix;
    gl_Position = mvp_matrix * vec4(vPosition, 1);

    Out.color = vColor;
}
