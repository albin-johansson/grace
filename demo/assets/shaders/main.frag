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
    oFragColor = vec4(1, 0, 0, 1);
}
