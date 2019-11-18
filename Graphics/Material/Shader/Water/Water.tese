#version 450 core
layout(quads) in;

layout(location = 0) uniform mat4 model;
layout(location = 1) uniform float t;

in vec3 ctlPoint[];
in vec2 texCoord[];

out vec2 texCoordFrag;

layout(std140)uniform _ProjMatrices
{
    mat4 view;
    mat4 projection;
};

vec3 interpolateQuad(in vec3 v0, in vec3 v1, in vec3 v2, in vec3 v3)
{
    vec3 a = mix(v0, v1, gl_TessCoord.x);
    vec3 b = mix(v3, v2, gl_TessCoord.x);
    return mix(a,b,gl_TessCoord.y);
}

vec2 interpolateQuad2D(in vec2 v0, in vec2 v1, in vec2 v2, in vec2 v3)
{
    vec2 a = mix(v0, v1, gl_TessCoord.x);
    vec2 b = mix(v3, v2, gl_TessCoord.x);
    return mix(a,b,gl_TessCoord.y);
}

float waveXdir(in vec2 pos, in vec2 D, in float Q, in float A, in float F, in float w, in float t)
{
    return pos.x + Q * A * D.x * cos(w * dot(D, pos) + F * t);
}

float waveZdir(in vec2 pos, in vec2 D, in float Q, in float A, in float F, in float w, in float t)
{
    return pos.y + Q * A * D.y * cos(w * dot(D, pos) + F * t);
}

float waveHeight(in vec2 pos, in vec2 D, in float A, in float F,in float w, in float t, in int k)
{
    return A * sin(w * dot(D, pos) + F * t);
}

const vec2 D1 = normalize(vec2(1.0,0.0));
const vec2 D2 = normalize(vec2(-.3,0.4));
const float w = 4.0;
const float A = 0.3;
const float F = 1.8;
const float Q = 1 / (w * A);
void main()
{
    vec3 pos = interpolateQuad(ctlPoint[0], ctlPoint[1], ctlPoint[2], ctlPoint[3]);
    float x = waveXdir(pos.xz, D1, Q, A, F, w, t);
    float z = waveZdir(pos.xz, D1, Q, A, F, w, t);
    float y = waveHeight(pos.xz, D1, A, F, w, t, 2);
    gl_Position = projection*view*model*vec4(vec3(x,y,z), 1.0);
}