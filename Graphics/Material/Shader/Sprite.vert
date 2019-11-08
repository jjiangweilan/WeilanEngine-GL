#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform mat4 view;
layout(location = 2) uniform mat4 projection;
layout(location = 3) uniform int hasTRigidbody;
layout(location = 4) uniform vec2 point1;
layout(location = 5) uniform vec2 point2;

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
