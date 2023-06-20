#version 330                          
layout (location = 0) in vec4 in_Vertex;

out Vertex{
    vec2 UV;
}v_out;

uniform float _Time;
uniform mat4 _Projection;

void main(){      
    v_out.UV = in_Vertex.zw;

    //Positions are in pixels
    vec2 pos = in_Vertex.xy;
    pos.y+=sin(pos.x / 64 + _Time * 4.0) * 4;
    gl_Position = _Projection * vec4(pos,0.0,1);
}
