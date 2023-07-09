#version 450 core

precision highp float;

layout (location = 0) in VsOutput {
    vec3 color;
} In;

layout (location = 0) out vec4 oFragColor;

void main()
{
    oFragColor = vec4(In.color, 1);
}
