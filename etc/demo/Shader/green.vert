#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

layout(location = 0) uniform mat4 model;

out vec3 normal;
out vec3 pos;
layout(std140) uniform _ProjMatrices
{
    mat4 view;
    mat4 projection;
};
void main()
{
    pos = aPos;
    normal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}