#version 450 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D texture0;

void main() {
    bool isOutline = false;
    for (int j = -1;j <=1;j++) {
        for (int k = -1;k <=1;k++) {
            if (texture(texture0, texCoord + vec2(j * 1.0 / 128.0,k * 1.0 / 128.0)).a > 0.3) {
                fragColor = vec4(.0,.0,.0,1.0);
                return;
            }
        }
    }
}