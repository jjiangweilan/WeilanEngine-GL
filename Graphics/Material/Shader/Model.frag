#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

layout (location = 4) uniform Light light1;
layout (location = 8) uniform Light light2;
layout (location = 12) uniform vec3 viewPos;
layout (location = 900) uniform sampler2D texture_diffuse[10];
layout (location = 910) uniform sampler2D texture_specular[10];
layout (location = 920) uniform sampler2D texture_ambient[10];
layout (location = 930) uniform sampler2D texture_normals[10];
layout (location = 940) uniform sampler2D texture_height[10];

vec3 calculateColor(Light light) {
    vec4 tex = texture(texture_diffuse[0], TexCoords);
    if (tex.x == 0 && tex.y == 0 && tex.z == 0) tex = vec4(0.5,0.5,0.5,1.0);
    // ambient
    vec3 ambient = light.ambient * tex.rgb;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * tex.rgb;  

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64.0);
    vec3 specular = light.specular * spec * tex.rgb;  

    return ambient + diffuse + specular;
}
void main()
{    
    Light light;
    light.position = vec3(5,5,5);
    light.diffuse = vec3(0.2,0.2,0.2);
    light.specular = vec3(0.6,0.6,0.6);
    light.ambient = vec3(0.4,0.4,0.4);
    vec3 rlt1 = calculateColor(light);
    //vec3 rlt2 = calculateColor(light2);
    FragColor =  vec4(rlt1,1.0);//texture(texture_diffuse[0], TexCoords);//vec4(rlt1 + rlt2, 1.0);
}
