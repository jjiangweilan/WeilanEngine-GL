#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

struct Light {
    vec3 position;

    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
};

uniform Light light1;
uniform Light light2;
uniform vec3 viewPos;

vec3 calculateColor(Light light) {
    vec4 tex = texture(texture_diffuse1, TexCoords);
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
    vec3 rlt1 = calculateColor(light1);
    vec3 rlt2 = calculateColor(light2);
    FragColor =  vec4(rlt1 + rlt2, 1.0);
}
