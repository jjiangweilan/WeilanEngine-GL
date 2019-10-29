#version 450 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D texture0;

void main() {
    vec4 colorTemp = vec4(0.0f, 0.0f, 0.0f, texture(texture0, texCoords).r);
    if (colorTemp.a == 0.0) discard;
    color = colorTemp;
}

