#version 430 core

in layout(location = 0) vec3 vPos;

uniform layout(location = 0) vec3 flyViewPos;
uniform layout(location = 1) mat4 P;

out layout(location = 0) vec3 viewPos;

void main()
{
    viewPos = vPos*0.1f + flyViewPos;
    gl_Position = P * vec4(viewPos, 1.0f);
}
