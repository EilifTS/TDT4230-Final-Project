#version 430 core

layout(binding = 0) uniform sampler2D colorEmSampler;
layout(binding = 1) uniform sampler2D normalDepthDiffuseSampler;
layout(binding = 2) uniform sampler2D firefly;
layout(binding = 3) uniform sampler2D diffuseSpecular;
layout(binding = 4) uniform sampler2D ambientSampler;

in layout(location = 0) vec4 pos_in;
in layout(location = 1) vec4 view_pos_in;

out vec4 color;

void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    float depth = texture(normalDepthDiffuseSampler, uv).z;
    vec3 fragPosVS = vec3(view_pos_in.xy, 1.0) * depth;
    vec3 firefly = texture(firefly, uv).xyz;
    vec3 albedo = texture(colorEmSampler, uv).xyz;
    vec3 diffuse = texture(diffuseSpecular, uv).xyz;

    float l = length(fragPosVS) / 10.0;
    float distanceFactor = 1.0 / (2.0 + 1.1 * l * l);

    float ambient = texture(ambientSampler, uv).x * distanceFactor;

    color = vec4(albedo * (ambient + diffuse) + firefly, 1.0);
}