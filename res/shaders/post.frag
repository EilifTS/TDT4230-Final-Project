#version 430 core

layout(binding = 0) uniform sampler2D colorEmSampler;
layout(binding = 1) uniform sampler2D normalDepthDiffuseSampler;
layout(binding = 2) uniform sampler2D diffuseSpecular;

in layout(location = 0) vec4 pos_in;

out vec4 color;

vec3 diffuseColor = vec3(173.0, 255.0, 47.0) / 255.0;


void main()
{
    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    //float diffusematerial = texture(normalDepthDiffuseSampler, uv).w;
    vec4 c_e = texture(colorEmSampler, uv);

    float diffuse = texture(diffuseSpecular, uv).x;
    float ambient = 0.3;

    color = vec4(c_e.xyz * (ambient + diffuse * diffuseColor), 1.0);
    //color = vec4(diffuse.xxx, 1.0);
}