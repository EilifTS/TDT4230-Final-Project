#version 430 core

layout(binding = 0) uniform sampler2D normalDepthSampler;

uniform layout(location = 0) vec3 lightViewPos;
uniform layout(location = 2) float phase;
uniform layout(location = 3) float maxIntensity;
uniform layout(location = 4) float maxRadius;


in layout(location = 0) vec4 pos_in;
in layout(location = 1) vec3 view_pos_in;

out layout(location = 0) vec4 light_out;

float la = 1;
float lb = 0.7;
float lc = 1.8;
float L(float d)
{
    return maxIntensity / (la + lb * d + lc * d * d);
}

vec3 lightColor1 = vec3(173.0, 255.0, 47.0) / 255.0;
vec3 lightColor2 = vec3(255.0, 73.0, 12.0) / 255.0;

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

        vec3 normal = vec3(normal_depth.xy, 0);
        normal.z = sqrt(1 - length(normal)*length(normal));
        light_out.xyz = clamp(-dot(normal, lightDir), 0, 1) * L(radius) * mix(lightColor1, lightColor2, phase);
    }
}
