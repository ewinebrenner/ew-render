#version 330                          
out vec4 FragColor;

uniform vec4 _Color;

in Vertex{
    vec2 UV;
}fs_in;

uniform sampler2D _FontTexture;

uniform int _DEBUG = 0;

void main(){         
    float a = texture(_FontTexture, fs_in.UV).r;
    if (_DEBUG == 1){
       FragColor = vec4(a,a,a,1.0);
       return;
    }
    a = smoothstep(0.495,0.505,a);
    FragColor = vec4(_Color.rgb,_Color.a * a);
}
