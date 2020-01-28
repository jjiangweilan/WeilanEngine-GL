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

layout (location = 4) uniform Light light1 = Light(vec3(0,3,3), vec3(0.2), vec3(0.7), vec3(0.1));
layout (location = 8) uniform Light light2;
layout(location = 900) uniform sampler2D diffuseTex0;

vec3 calculateColor(Light light) {
    vec4 tex = texture(diffuseTex0, TexCoords);
    if (dot(tex.rgb, vec3(1.0,1.0,1.0)) == 0.0) tex = vec4(0.5,0.5,0.5,1.0);
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
    vec3 rlt1=calculateColor(light1);

    FragColor = vec4(texture(diffuseTex0, TexCoords).rgb,1.0);// vec4(rlt1,1.0);//texture(texture_diffuse[0], TexCoords);//vec4(rlt1 + rlt2, 1.0);
}
