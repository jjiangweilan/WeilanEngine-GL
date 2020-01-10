
#version 450 core
layout (location = 0) in vec2 inPos;
out vec2 outPos;
uniform mat4 projection;
uniform mat4 view;
void main()
{
    gl_Position = projection * view * vec4(inPos, 100000.0, 1.0f);
};
