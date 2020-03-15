#version 430 core

layout(binding = 0) uniform sampler2D colorSampler;


in layout(location = 0) vec4 pos_in;

out vec4 color;

void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec3 c = texture(colorSampler, uv).xyz;
    color = vec4(c, 1.0);
}