#version 430 core

in layout(location = 0) vec3 vPos;
in layout(location = 1) vec3 normal_in;
in layout(location = 2) vec2 textureCoordinates_in;

uniform layout(location = 0) vec3 lightViewPos;
uniform layout(location = 1) mat4 P;
uniform layout(location = 2) float maxIntensity;
uniform layout(location = 3) float maxRadius;


out layout(location = 0) vec4 pos_out;
out layout(location = 1) vec3 view_pos_out;

void main()
{
    view_pos_out = vPos * maxRadius + lightViewPos;
    pos_out = P * vec4(view_pos_out, 1.0f);
    gl_Position = pos_out;
    pos_out.xyz /= pos_out.w;
    view_pos_out /= view_pos_out.z;
}
