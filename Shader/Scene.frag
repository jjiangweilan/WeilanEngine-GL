#version 450 core
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D gameSceneSampler;
uniform sampler2D volumetricLightSampler;
void main() {
    vec4 gameSceneFrag = texture(gameSceneSampler, texCoord);
    vec4 vlFrag = texture(volumetricLightSampler, texCoord);

    if (vlFrag.a < 0.1) fragColor = gameSceneFrag;
    else {
        fragColor=gameSceneFrag+vlFrag;
    }

}