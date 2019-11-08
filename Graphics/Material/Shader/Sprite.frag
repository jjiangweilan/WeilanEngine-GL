#version 450 core
in vec2 texCoords;
out vec4 fragColor;

layout(location = 1000) uniform sampler2D texture0;
layout(location = 1001) uniform sampler2D texture1;
layout(location = 1002) uniform sampler2D texture2;
layout(location = 1004) uniform sampler2D texture3;
layout(location = 7) uniform float transparency;

void main() {
    vec4 fragColorTemp = texture(texture0, texCoords);
    if(fragColorTemp.a < 0.1) discard;
    fragColorTemp.a = transparency;
    
    fragColor = fragColorTemp;
}

