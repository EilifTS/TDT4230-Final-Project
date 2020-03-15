#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 normal_in;
in layout(location = 2) vec2 textureCoordinates_in;


uniform layout(location = 0) mat4 MV;
uniform layout(location = 1) mat4 MVP;

out layout(location = 0) vec4 view_pos_out;
out layout(location = 1) vec3 normal_out;
out layout(location = 2) vec2 textureCoordinates_out;

void main()
{
    normal_out = normalize(mat3(MV) * normal_in);

    textureCoordinates_out = textureCoordinates_in;
    gl_Position = MVP * vec4(position, 1.0f);
    view_pos_out = MV * vec4(position, 1.0f);
}
