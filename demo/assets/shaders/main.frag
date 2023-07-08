#version 450 core

precision highp float;

layout (location = 0) in VsOutput {
    vec3 ws_position;
    vec3 vs_position;
    vec3 vs_normal;
    vec2 tex_coords;
} In;

layout (location = 0) out vec4 oFragColor;

void main()
{
    oFragColor = vec4(0.21f, 0.36f, 0.78f, 1);
}
