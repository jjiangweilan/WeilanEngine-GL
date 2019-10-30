#version 450 core
in vec2 texCoords;
in vec2 fragPos;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture_normal;
uniform vec2 lightPos;

void main() {
    vec4 fragColorTemp = texture(texture0, texCoords);
    vec3 normal = texture(texture_normal, texCoords).rgb;

    if(fragColorTemp.a < 1) discard;
    vec4 fragColor = fragColorTemp;
    if(normal.r < 0.1 && normal.g < 0.1 && normal.b < 0.1) {
        FragColor = fragColor;
    }
    else {
        vec3 lightColor = vec3(0.6);
        // ambient
        float ambientStrength = 0.4;
        vec3 ambient = vec3(ambientStrength);

        // diffuse 
        vec3 norm = normalize(normal * 2 - 1);
        vec3 lightDir = normalize(vec3(lightPos.x,0.0,lightPos.y) - vec3(fragPos.x,0.0, fragPos.y));
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        vec3 result = (ambient + diffuse) * fragColor.rgb;
        FragColor = vec4(result, 1.0);
    }
}
