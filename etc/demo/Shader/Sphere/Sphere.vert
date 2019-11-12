#version 450 core

layout(location = 0) in vec3 in_pos;
layout(location = 2) in vec2 aTexCoord;

out vec3 vert_pos;
out vec2 texCoord;
void main()
{
    vert_pos = in_pos;
    texCoord = aTexCoord;
}