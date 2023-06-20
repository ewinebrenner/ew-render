#version 330                          
layout (location = 0) in vec3 in_Pos;  
layout (location = 1) in int in_TextureIndex;
layout (location = 2) in vec4 in_Color;
layout (location = 3) in vec2 in_UV;

uniform mat4 _MVP;

out Vertex{
    vec2 UV;
    int textureIndex;
    vec4 color;
}v_out;

void main(){      
    v_out.UV = in_UV;
    v_out.color = in_Color;
    v_out.textureIndex = in_TextureIndex;
    gl_Position = _MVP * vec4(in_Pos,1);
}
