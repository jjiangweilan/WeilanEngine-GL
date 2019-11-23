#version 450 core

in vec2 texCoordFrag;
out vec4 fragColor;

uniform sampler2D diffuseTex0;
void main()
{
    fragColor = texture(diffuseTex0, texCoordFrag);
}