#version 330                          
out vec4 FragColor;

in Vertex{
    vec2 UV;
    int textureIndex;
    vec4 color;
}fs_in;

uniform sampler2D _Textures[8];

void main(){         
    FragColor = texture(_Textures[fs_in.textureIndex], fs_in.UV) * fs_in.color;
}
