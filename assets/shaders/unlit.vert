#version 330                          
layout (location = 0) in vec3 in_Pos;  
layout (location = 1) in vec3 in_Normal;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

out vec3 Normal;

void main(){       
    Normal = in_Normal;
    gl_Position = _Projection * _View * _Model * vec4(in_Pos,1);
}
