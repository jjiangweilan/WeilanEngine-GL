#version 450 core
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D texture0;
void main() {
    fragColor = texture(texture0, texCoord);
}