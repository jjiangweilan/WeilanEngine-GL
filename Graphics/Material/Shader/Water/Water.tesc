#version 450 core
layout(vertices = 4) out;
in vec3 vertPos[];
in vec2 texCoord_[];

out vec3 ctlPoint[];
out vec2 texCoord[];
void main()
{
    gl_TessLevelOuter[0] = 32.0;
    gl_TessLevelOuter[1] = 32.0;
    gl_TessLevelOuter[2] = 32.0;
    gl_TessLevelOuter[3] = 32.0;

    gl_TessLevelInner[0] = 32.0;
    gl_TessLevelInner[1] = 32.0;

    ctlPoint[gl_InvocationID] = vertPos[gl_InvocationID];
    texCoord[gl_InvocationID] = texCoord_[gl_InvocationID];
}