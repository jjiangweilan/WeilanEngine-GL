#version 450 core
in vec2 texCoords;
out vec4 fragColor;

uniform sampler2D texture0;
uniform float transparency;

void main() {
    vec4 fragColorTemp = texture(texture0, texCoords);
    if(fragColorTemp.a < 0.1) discard;
    fragColorTemp.a = transparency;
    
    fragColor = fragColorTemp;
}

