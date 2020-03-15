#version 430 core

layout(binding = 0) uniform sampler2D posSampler;
layout(binding = 1) uniform sampler2D normalSampler;
layout(binding = 2) uniform sampler2D colorSampler;
layout(binding = 3) uniform sampler2D diffuseSampler;

in layout(location = 0) vec4 pos_in;

out vec4 color;

vec3 diffuseColor = vec3(173.0, 255.0, 47.0) / 255.0;


void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec4 pos = texture(posSampler, uv);
    vec3 normal = texture(normalSampler, uv).xyz;
    vec3 c = texture(colorSampler, uv).xyz;

    float diffuse = texture(diffuseSampler, uv).x;
    float ambient = 0.3;

    color = vec4(c * (ambient + diffuse * diffuseColor), 1.0);
}