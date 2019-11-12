#version 450 core

in vec3 normal;
in vec3 pos;

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

layout (location = 12) uniform vec3 viewPos;

vec3 calculateColor(Light light) {
    vec4 tex = vec4(1.0, 0.9059, 0.4902, 1.0);
    if (tex.x == 0 && tex.y == 0 && tex.z == 0) tex = vec4(0.5,0.5,0.5,1.0);
    // ambient
    vec3 ambient = light.ambient * tex.rgb;

    // diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(light.position - pos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex.rgb;  

    // specular
    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = light.specular * spec * tex.rgb;  

    return ambient + diffuse + specular;
}

void main() {
    Light light;
    light.position = vec3(5,5,5);
    light.diffuse = vec3(0.5,0.5,0.5);
    light.specular = vec3(0.4,0.4,0.4);
    light.ambient = vec3(0.2,0.2,0.2);
    vec3 rlt1 = calculateColor(light);
    light.position = vec3(-5,-5,-5);
    vec3 rlt2 = calculateColor(light);
    gl_FragColor = vec4(rlt1 + rlt2, 1.0);
}
