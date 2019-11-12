#version 450 core

layout(isolines,equal_spacing,ccw)in;

layout(location = 0) uniform mat4 model;

in vec4 tesc_out_pos[];

layout(std140) uniform _ProjMatrices
{
    mat4 view;
    mat4 projection;
};

vec4 interpolate(in vec4 v0,in vec4 v1,in vec4 v2,in vec4 v3)
{
    vec4 a=mix(v0,v1,gl_TessCoord.x);
    vec4 b=mix(v3,v2,gl_TessCoord.x);
    return mix(a,b,gl_TessCoord.y);
}
void main()
{
    float u = gl_TessCoord.x;
    vec4 p0 = tesc_out_pos[0];
    vec4 p1 = tesc_out_pos[1];
    vec4 pos = vec4(1.0);
float leng=length(p1-p0)/2.;
    pos.x = tesc_out_pos[0].x * u + tesc_out_pos[1].x * (1-u);
    pos.y = tesc_out_pos[0].y + leng * sin(u * 2 * 3.1415);
    pos.z = 0.0;

    gl_Position = projection * view * model * pos;
}