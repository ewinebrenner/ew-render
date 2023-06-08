#version 450 core                          
layout (location = 0) in vec3 in_Pos;  
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;

out Vertex{
    vec2 UV;
}vs_out;

void main(){      
    vs_out.UV = in_UV;
    gl_Position = vec4(in_Pos,1);
}