#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;

out layout(location = 0) vec4 pos_out;

void main()
{
    pos_out = vec4(position, 1.0f);
    gl_Position = pos_out;
}
