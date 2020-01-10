#version 450 core
layout(points) in;
layout(line_strip, max_vertices=30) out;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform vec3 i; //min
layout(location = 2) uniform vec3 a; //max

layout(std140) uniform _ProjMatrices
{
    mat4 view;
    mat4 projection;
};

void main()
{
    mat4 trans = projection * view * model;

    vec4 p0 = trans * vec4(vec3(a), 1.0);
    vec4 p1 = trans * vec4(vec3(i.x,a.y,a.z), 1.0);
    vec4 p2 = trans * vec4(vec3(i.x,a.y,i.z), 1.0);
    vec4 p3 = trans * vec4(vec3(a.x,a.y,i.z), 1.0);

    vec4 p4 = trans * vec4(vec3(a.x,i.y,a.z), 1.0);
    vec4 p5 = trans * vec4(vec3(i.x,i.y,a.z), 1.0);
    vec4 p6 = trans * vec4(vec3(i), 1.0);
    vec4 p7 = trans * vec4(vec3(a.x,i.y,i.z), 1.0);

//front
    gl_Position = p0;
    EmitVertex();
    gl_Position = p4;
    EmitVertex();
    gl_Position = p5;
    EmitVertex();
    gl_Position = p1;
    EmitVertex();
    gl_Position = p0;
    EmitVertex();
    EndPrimitive();

//left
    gl_Position = p1;
    EmitVertex();
    gl_Position = p5;
    EmitVertex();
    gl_Position = p6;
    EmitVertex();
    gl_Position = p2;
    EmitVertex();
    gl_Position = p1;
    EmitVertex();
    EndPrimitive();

//right
    gl_Position = p3;
    EmitVertex();
    gl_Position = p7;
    EmitVertex();
    gl_Position = p4;
    EmitVertex();
    gl_Position = p0;
    EmitVertex();
    gl_Position = p3;
    EmitVertex();
    EndPrimitive();

//top
    gl_Position = p3;
    EmitVertex();
    gl_Position = p0;
    EmitVertex();
    gl_Position = p1;
    EmitVertex();
    gl_Position = p2;
    EmitVertex();
    gl_Position = p3;
    EmitVertex();
    EndPrimitive();

//back
    gl_Position = p3;
    EmitVertex();
    gl_Position = p7;
    EmitVertex();
    gl_Position = p6;
    EmitVertex();
    gl_Position = p2;
    EmitVertex();
    gl_Position = p3;
    EmitVertex();
    EndPrimitive();

//down
    gl_Position = p7;
    EmitVertex();
    gl_Position = p4;
    EmitVertex();
    gl_Position = p5;
    EmitVertex();
    gl_Position = p6;
    EmitVertex();
    gl_Position = p7;
    EmitVertex();
    EndPrimitive();
}