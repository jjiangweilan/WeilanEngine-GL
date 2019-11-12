#version 450 core

in vec2 texCoordFrag;
out vec4 fragColor;

layout(location = 900) uniform sampler2D texture_diffuse[10];
void main()
{
    fragColor = texture(texture_diffuse[0], texCoordFrag);
}