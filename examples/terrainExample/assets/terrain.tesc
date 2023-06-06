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
uniform mat4 _View;
uniform mat4 _Model;
uniform mat4 _Projection;
uniform float _TessLevelOuter;
uniform float _TessLevelInner;
void main(){
    //Tesselation levels
    if (gl_InvocationID == 0){
//        vec4 p0 = _Projection * _View * _Model * gl_in[0].gl_Position;
//        vec4 p1 = _Projection * _View * _Model * gl_in[1].gl_Position;
//        vec4 p2 = _Projection * _View * _Model * gl_in[2].gl_Position;
//        vec4 p3 = _Projection * _View * _Model * gl_in[3].gl_Position;
//        p0/=p0.w;
//        p1/=p1.w;
//        p2/=p2.w;
//        p3/=p3.w;
//        //Don't tesselate behind camera
//        if (p0.z <= 0.0 || p1.z <= 0.0 || p2.z <= 0.0 || p3.z <= 0.0){
//            gl_TessLevelOuter[0] =  gl_TessLevelOuter[1] =  gl_TessLevelOuter[2] = gl_TessLevelOuter[3] = gl_TessLevelInner[0] = 0.0;
//        }else{
//            //Measure XY distance in screen space
//            float l0 = max(length(p1.xy-p2.xy) * _TessLevelPerUnit,_MinTessLevel);
//            float l1 = max(length(p2.xy-p0.xy) * _TessLevelPerUnit,_MinTessLevel);
//            float l2 = max(length(p0.xy-p1.xy) * _TessLevelPerUnit,_MinTessLevel);
//            gl_TessLevelOuter[0] = l0;
//            gl_TessLevelOuter[1] = l1;
//            gl_TessLevelOuter[2] = l2;
//            gl_TessLevelInner[0] = min(min(l0,l1),l2);
//        }
            gl_TessLevelOuter[0] = _TessLevelOuter;
            gl_TessLevelOuter[1] = _TessLevelOuter;
            gl_TessLevelOuter[2] = _TessLevelOuter;
            gl_TessLevelOuter[3] = _TessLevelOuter;
            gl_TessLevelInner[0] = _TessLevelInner;
            gl_TessLevelInner[1] = _TessLevelInner;
    }

    //Pass attributes through
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tc_out[gl_InvocationID] = tc_in[gl_InvocationID];
}