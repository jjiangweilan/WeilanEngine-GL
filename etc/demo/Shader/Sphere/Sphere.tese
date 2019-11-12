
#version 450 core
layout(location=0)uniform mat4 model;
layout(quads,equal_spacing,ccw)in;

in vec3 ctrl_point[];
in vec2 outTexCoord[];

out vec2 texCoordFrag;


layout(std140)uniform _ProjMatrices
{
    mat4 view;
    mat4 projection;
};

vec3 normalize(vec3 A,vec3 B,float length)
{
    vec3 delta=B-A;
    delta=delta*length/distance(A,B);
    return A+delta;
}

//PG produces a standalone unit 
vec3 interpolateQuad(in vec3 v0,in vec3 v1,in vec3 v2,in vec3 v3)
{
    vec3 a=mix(v1,v2,gl_TessCoord.x);
    vec3 b=mix(v0,v3,gl_TessCoord.x);
    return mix(a,b,gl_TessCoord.y);
}

vec2 interpolateQuad2D(in vec2 v0,in vec2 v1,in vec2 v2,in vec2 v3)
{
    vec2 a=mix(v1,v2,gl_TessCoord.x);
    vec2 b=mix(v0,v3,gl_TessCoord.x);
    return mix(a,b,gl_TessCoord.y);
}

vec3 interpolateTriangle(vec3 pos0,vec3 pos1,vec3 pos2)
{
    return vec3(gl_TessCoord.x)*pos0+vec3(gl_TessCoord.y)*pos1+vec3(gl_TessCoord.z)*pos2;
}
vec2 interpolateTriangle2D(vec2 pos0,vec2 pos1,vec2 pos2)
{
    return gl_TessCoord.x*pos0+gl_TessCoord.y*pos1+gl_TessCoord.z*pos2;
}
void main()
{
    vec3 pos=interpolateQuad(ctrl_point[0],ctrl_point[1],ctrl_point[2], ctrl_point[3]);
    texCoordFrag = interpolateQuad2D(outTexCoord[0],outTexCoord[1],outTexCoord[2], outTexCoord[3]);
    
    gl_Position=projection*view*model*vec4(normalize(vec3(0,0,0),pos,0.5),1.);
}