#version 450 core
layout (vertices = 4) out;
in Vertex{
    vec2 UV;
}tc_in[];

out Vertex{
    vec2 UV;
}tc_out[];

uniform float _MinTessLevel = 1;
uniform float _TessLevelPerUnit = 16;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

uniform float _TessLevelOuter;
uniform float _TessLevelInner;

uniform sampler2D _HeightMap;
uniform float _Frequency;
uniform float _Amplitude;
uniform vec2 _ViewportSize;

float evaluateHeight(vec2 uv){ 
    return texture(_HeightMap,uv * _Frequency).r * _Amplitude;
}

//Distance based LOD algorithm originally from DirectX 11 Tesselation by Iain Cantlay 
//https://picture.iczhiku.com/resource/paper/syIWkyWiwiEkWNCc.pdf
//Ported to GLSL by Victor Bush
//https://victorbush.com/2015/01/tessellated-terrain/
float distTess(vec4 p0, vec4 p1, vec2 uv0, vec2 uv1){
    float tesselatedTriSize = _TessLevelOuter;

    //Adjust height so that it matches final position in terrain.tese
    p0.y+=evaluateHeight(uv0);
    p1.y+=evaluateHeight(uv1);

    //Define sphere in center of edge in local space
    vec4 center = 0.5 * (p0 + p1);
    float radius = distance(p0,p1) * 0.5;

    vec4 sc0 = _View * _Model * center;
    vec4 sc1 = sc0;
    sc0.x -= radius;
    sc1.x += radius;

    //Projected sphere edge
    vec4 clip0 = _Projection * sc0;
    vec4 clip1 = _Projection * sc1;
    clip0/=clip0.w;
    clip1/=clip1.w;
    clip0.xy*=_ViewportSize;
    clip1.xy*=_ViewportSize;
    float d = distance(clip0,clip1);
    return clamp(d/tesselatedTriSize,0,64);
}

void main(){
    //Tesselation levels
    if (gl_InvocationID == 0){
            //Outer left edge
            gl_TessLevelOuter[0] = distTess(gl_in[3].gl_Position,gl_in[0].gl_Position,tc_in[3].UV,tc_in[0].UV);
            //Outer bottom edge
            gl_TessLevelOuter[1] = distTess(gl_in[0].gl_Position,gl_in[1].gl_Position,tc_in[0].UV,tc_in[1].UV);
            //Outer right edge
            gl_TessLevelOuter[2] = distTess(gl_in[1].gl_Position,gl_in[2].gl_Position,tc_in[1].UV,tc_in[2].UV);
            //Outer top edge
            gl_TessLevelOuter[3] = distTess(gl_in[2].gl_Position,gl_in[3].gl_Position,tc_in[2].UV,tc_in[3].UV);

            //Horizontal inner - average of left and top
            gl_TessLevelInner[0] = 0.5 * (gl_TessLevelOuter[0] + gl_TessLevelOuter[3]);
            //Vertical inner - average of right and bottom
            gl_TessLevelInner[1] = 0.5 * (gl_TessLevelOuter[2] + gl_TessLevelOuter[1]);
    }

    //Pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tc_out[gl_InvocationID] = tc_in[gl_InvocationID];
}