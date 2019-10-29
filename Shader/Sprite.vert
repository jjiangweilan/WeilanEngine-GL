#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform int hasTRigidbody;
uniform vec2 point1;
uniform vec2 point2;

out vec2 texCoords;
void main()
{
    texCoords = aTexCoords;

    vec4 pos = model * vec4(aPos, 1.0);
    if (hasTRigidbody == 1) {
        float zz = (pos.x-point1.x)*(point2.y-point1.y)/(point2.x-point1.x) + point1.y;
        pos.z = -zz;
    }
    gl_Position = projection * view * pos;
}
