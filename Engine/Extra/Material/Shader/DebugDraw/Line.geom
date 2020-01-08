#version 450 core
layout(points) in;
layout(line_strip, max_vertices = 2) out;

layout(std140) uniform _ProjMatrices
{
    mat4 view;
    mat4 projection;
};

layout(location = 0) uniform vec3 from;
layout(location = 1) uniform vec3 to;
void main()
{
    gl_Position = projection * view * vec4(from, 1.0);
    EmitVertex();

    gl_Position = projection * view * vec4(to, 1.0);
    EmitVertex();

    EndPrimitive();
}