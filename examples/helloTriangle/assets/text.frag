#version 330                          
out vec4 FragColor;

uniform vec4 _Color;

in Vertex{
    vec2 UV;
}fs_in;

uniform sampler2D _FontTexture;

void main(){         
    float a = texture(_FontTexture, fs_in.UV).r;
    FragColor = vec4(_Color.rgb,_Color.a * a);
}
