#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

layout (std140) uniform GlobalProjMatrices
{
    mat4 view;
    mat4 projection;
};

layout (location = 0) uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;    
    Normal = mat3(inverse(transpose(model))) * aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(FragPos,1.0);
}
