#version 330                          
layout (location = 0) in vec4 in_Vertex;

uniform mat4 _Projection;

out Vertex{
    vec2 UV;
}v_out;

void main(){      
    v_out.UV = in_Vertex.zw;
    gl_Position = _Projection * vec4(in_Vertex.xy,0.0,1);
}
