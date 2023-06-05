#version 450 core
layout (vertices = 3) out;
in Vertex{
    vec2 UV;
}tc_in[];

out Vertex{
    vec2 UV;
}tc_out[];

uniform float _TessLevelOuter = 8;
uniform float _TessLevelInner = 5;

void main(){
    //Tesselation levels
    if (gl_InvocationID == 0){
        gl_TessLevelInner[0] = _TessLevelInner;
        gl_TessLevelOuter[0] = _TessLevelOuter;
	    gl_TessLevelOuter[1] = _TessLevelOuter;
	    gl_TessLevelOuter[2] = _TessLevelOuter;
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tc_out[gl_InvocationID] = tc_in[gl_InvocationID];
}