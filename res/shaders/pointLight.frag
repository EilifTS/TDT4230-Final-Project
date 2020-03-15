#version 430 core

layout(binding = 0) uniform sampler2D posSampler;
layout(binding = 1) uniform sampler2D normalSampler;

uniform layout(location = 0) vec3 lightViewPos;
uniform layout(location = 2) float maxIntensity;
uniform layout(location = 3) float maxRadius;

in layout(location = 0) vec4 pos_in;

out layout(location = 0) vec4 light_out;

float la = 1;
float lb = 0.7;
float lc = 1.8;
float L(float d)
{
    return 1.0 / (la + lb * d + lc * d * d);
}

void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec4 pos = texture(posSampler, uv);
    vec3 normal = texture(normalSampler, uv).xyz;

    vec3 lightDir = pos.xyz - lightViewPos;
    float radius = length(lightDir);
    lightDir = normalize(lightDir);

    light_out = vec4(0,0,0,1);

    if(radius < maxRadius)
        light_out.x = clamp(-dot(normal, lightDir), 0, 1) * L(radius);
}
