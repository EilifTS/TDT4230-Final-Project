#version 430 core

uniform layout(location = 0) vec3 flyViewPos;

in layout(location = 0) vec3 viewPos;
out layout(location = 0) vec4 color;


float la = 0.01f;
float lb = 10.0f;
float lc = 100.0f;
vec3 flyColor = vec3(173.0, 255.0, 47.0) / 255.0;

void main()
{
    float radius = length(vec3(viewPos.xy, flyViewPos.z) - flyViewPos) / 0.05;

    float L = 1.0f / (la + lb * radius + lc * radius * radius);

    /*if(flyViewPos.z < -5)
        L *= 1 + -(flyViewPos.z + 10) * 0.4f;*/
    if(flyViewPos.z < -10)
        L *= -(flyViewPos.z) * 0.2f;

    color = vec4(flyColor * L, 0.0);
}
