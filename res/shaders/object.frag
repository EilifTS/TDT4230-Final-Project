#version 430 core

layout(binding = 0) uniform sampler2D tSampler;

uniform layout(location = 2) vec4 diffuse_emmisive;

in layout(location = 0) vec4 view_pos;
in layout(location = 1) vec3 normal;
in layout(location = 2) vec2 textureCoordinates;

out layout(location = 0) vec4 color_em_out;
out layout(location = 1) vec4 normal_depth_diff_out;

void main()
{
    color_em_out = vec4(texture(tSampler, textureCoordinates).xyz, diffuse_emmisive.y);
    vec3 normal2 = normalize(normal);
    normal_depth_diff_out = vec4(normal2.xy, view_pos.z, diffuse_emmisive.x);
}
