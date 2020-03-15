#version 430 core

layout(binding = 0) uniform sampler2D posSampler;
layout(binding = 1) uniform sampler2D normalSampler;
layout(binding = 2) uniform sampler2D colorSampler;

in layout(location = 0) vec4 pos_in;

out vec4 color;

vec3 lightDir = vec3(1.0, -1.0, 0.0);

void main()
{
    vec3 l = normalize(lightDir);

    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec3 pos = texture(posSampler, uv).xyz;
    vec3 normal = texture(normalSampler, uv).xyz;
    vec3 c = texture(colorSampler, uv).xyz;

    float diffuse = clamp(-dot(normal, l), 0, 1) * 0.3;
    float ambient = 0.7;

    color = vec4(c * (ambient + diffuse), 1.0);
}