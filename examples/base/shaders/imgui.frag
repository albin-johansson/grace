#version 450 core

precision highp float;

layout (location = 0) in VsOutput {
    vec2 uv;
    vec4 color;
} In;

layout (location = 0) out vec4 oFragColor;

layout (binding = 0) uniform sampler2D uFontSampler;

void main()
{
    oFragColor = In.color * texture(uFontSampler, In.uv);
}