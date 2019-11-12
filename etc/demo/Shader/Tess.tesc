#version 450 core

layout(vertices=2)out;

out vec4 tesc_out_pos[];

void main()
{
    gl_TessLevelOuter[0]=1.;
    gl_TessLevelOuter[1]=64.;
    tesc_out_pos[gl_InvocationID] = gl_in[gl_InvocationID].gl_Position;
}