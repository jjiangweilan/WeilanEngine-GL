#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoord;

out vec3 vertPos;
out vec2 texCoord_;
void main()
{
    vertPos = aPos;
    texCoord_ = aTexCoord;
}