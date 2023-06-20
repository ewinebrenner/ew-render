#version 330                          
layout (location = 0) in vec3 in_Pos;  
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out Vertex{
    vec3 Normal;
    vec2 UV;
}v_out;

void main(){      
    v_out.Normal = in_Normal;
    v_out.UV = in_UV;
    gl_Position = _Projection * _View * _Model * vec4(in_Pos,1);
}
