#version 450 core
layout(vertices = 4) out;

in vec3 vert_pos[];
in vec2 texCoord[];
out vec3 ctrl_point[];
out vec2 outTexCoord[];

void main()
{
    gl_TessLevelOuter[0] = 8.0;
    gl_TessLevelOuter[1] = 8.0;
    gl_TessLevelOuter[2] = 8.0;
    gl_TessLevelOuter[3] = 8.0;

    gl_TessLevelInner[0] = 16.0;
    gl_TessLevelInner[1] = 16.0;

    ctrl_point[gl_InvocationID] = vert_pos[gl_InvocationID];
    outTexCoord[gl_InvocationID] = texCoord[gl_InvocationID];
}