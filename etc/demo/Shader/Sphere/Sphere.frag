#version 450 core

in vec2 texCoordFrag;
out vec4 fragColor;

layout(location = 900) uniform sampler2D texture_diffuse;
void main()
{
    fragColor = texture(texture_diffuse, texCoordFrag);
}