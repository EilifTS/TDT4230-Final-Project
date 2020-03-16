#version 430 core

layout(binding = 0) uniform sampler2D normalDepthSampler;

uniform layout(location = 0) vec3 lightViewPos;
uniform layout(location = 2) float maxIntensity;
uniform layout(location = 3) float maxRadius;

in layout(location = 0) vec4 pos_in;
in layout(location = 1) vec3 view_pos_in;

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
    light_out = vec4(0,0,0,1);

    vec2 uv = (pos_in.xy + vec2(1.0, 1.0)) * 0.5;
    vec3 normal_depth = texture(normalDepthSampler, uv).xyz;

    //light_out.x = view_pos_in.x;
    //return;

    vec3 pos = view_pos_in * normal_depth.z;
    vec3 lightDir = pos.xyz - lightViewPos;
    float radius = length(lightDir);
    if(radius < maxRadius)
    {
        lightDir = normalize(lightDir);

        vec3 normal = normalize(vec3(normal_depth.xy, 1));
        light_out.x = clamp(-dot(normal, lightDir), 0, 1) * L(radius);
    }
}
