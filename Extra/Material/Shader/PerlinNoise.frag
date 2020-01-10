#version 450 core

out f2v
{
    vec3 worldPos;
    vec2 uvPos;
} i;

float fade(float i)
{
    return i*i*i*(10.+i*(6.*i-15.));
}

vec2 hash(vec2 co){
    float m=dot(co,vec2(12.9898,78.233));
    return frac(vec2(sin(m),cos(m))*43758.5453)*2.-1.;
}

uniform vec2 _CellNum = vec2(2,2);

float PerlinNoise(vec2 uv,vec2 offset) // offset: can be offset by time
{
    vec2 xy=(uv+vec2(offset.x,offset.y))*_CellNum;
    int2 sti=floor(xy);
    vec2 stf=fract(xy);
    
    float u=stf.x;
    float v=stf.y;
    
    float aa=dot(hash(sti+vec2(0,0)),vec2(u,v));
    float ab=dot(hash(sti+vec2(1,0)),vec2(u-1.,v));
    float ba=dot(hash(sti+vec2(0,1)),vec2(u,v-1.));
    float bb=dot(hash(sti+vec2(1,1)),vec2(u-1.,v-1.));
    
    u=fade(u);
    v=fade(v);
    
    float val=_Brightness*(mix(mix(aa,ab,u),mix(ba,bb,u),v)+1)/2;
    return val;
}

void main()
{
    
}