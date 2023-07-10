#version 450 core

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec4 vColor;

layout (location = 0) out VsOutput {
    vec2 uv;
    vec4 color;
} Out;

layout (std140, push_constant) uniform PushConstants {
    vec2 uScale;
    vec2 uTranslate;
};

void main()
{
    gl_Position = vec4(vPosition * uScale + uTranslate, 0.0, 1.0);

    Out.uv = vUV;
    Out.color = vColor;
}