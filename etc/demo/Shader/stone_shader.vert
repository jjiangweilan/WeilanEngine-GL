#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoords;
out vec2 fragPos;
void main()
{
    texCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = gl_Position.xy;
}
