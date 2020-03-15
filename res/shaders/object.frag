#version 430 core

layout(binding = 0) uniform sampler2D tSampler;

uniform layout(location = 2) vec4 diffuse_emmisive;

in layout(location = 0) vec4 view_pos;
in layout(location = 1) vec3 normal;
in layout(location = 2) vec2 textureCoordinates;

out layout(location = 0) vec4 pos_out;
out layout(location = 1) vec4 normal_out;
out layout(location = 2) vec4 color_out;
out layout(location = 3) vec4 light_prop_out;

void main()
{
    pos_out = view_pos;
    normal_out = vec4(normalize(normal), 1.0);
    color_out = texture(tSampler, textureCoordinates);
    light_prop_out = vec4(diffuse_emmisive.xy, 0.0, 0.0);
}
