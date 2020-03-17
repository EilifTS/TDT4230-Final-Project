#version 430 core

layout(binding = 0) uniform sampler2D colorEmSampler;
layout(binding = 1) uniform sampler2D normalDepthDiffuseSampler;
layout(binding = 2) uniform sampler2D firefly;
layout(binding = 3) uniform sampler2D diffuseSpecular;

in layout(location = 0) vec4 pos_in;

out vec4 color;

//vec3 diffuseColor = vec3(173.0, 255.0, 47.0) / 255.0;


void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec3 normal = vec3(texture(normalDepthDiffuseSampler, uv).xy, 0);
    vec3 firefly = texture(firefly, uv).xyz;
    normal.z = sqrt(1 - length(normal)*length(normal));

    vec4 c_e = texture(colorEmSampler, uv);

    vec3 diffuse = texture(diffuseSpecular, uv).xyz;
    float ambient = 0.0;

    color = vec4(c_e.xyz * (ambient + diffuse) + firefly, 1.0);
    //color = vec4(normal.xyz, 1.0);
}