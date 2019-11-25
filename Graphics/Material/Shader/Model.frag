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

layout (std140) uniform MainCamera
{
    vec3 viewPos;
};

layout (location = 4) uniform Light light1;
layout (location = 8) uniform Light light2;
layout(location = 900) uniform sampler2D diffuseTex0;

vec3 calculateColor(Light light) {
    vec4 tex = texture(diffuseTex0, TexCoords);

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

    return ambient * 0.1 + diffuse + specular * 10;
}
void main()
{    
    Light light;
    light.position = vec3(0,3,3);
    light.diffuse = vec3(0.2,0.2,0.2);
    light.specular = vec3(1.0, 1.0, 1.0);
    light.ambient = vec3(0.4,0.4,0.4);
    vec3 rlt1 = calculateColor(light);
    //vec3 rlt2 = calculateColor(light2);
    FragColor =  vec4(rlt1,1.0);//texture(texture_diffuse[0], TexCoords);//vec4(rlt1 + rlt2, 1.0);
}
