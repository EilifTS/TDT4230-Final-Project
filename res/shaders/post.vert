#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;

out layout(location = 0) vec4 pos_out;
out layout(location = 1) vec4 view_pos_out;

uniform layout(location = 0) mat4 inv_projection;

void main()
{
    pos_out = vec4(position.xy, 1.0f, 1.0f);
    view_pos_out = -inv_projection * pos_out;
    gl_Position = pos_out;
}
