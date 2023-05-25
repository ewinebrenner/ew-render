#version 330                          
out vec4 FragColor;

uniform vec3 _Color;

in Vertex{
    vec3 Normal;
    vec2 UV;
}fs_in;

uniform sampler2D _Texture;
uniform sampler2D _NormalMap;

void main(){         
    FragColor = texture(_NormalMap, fs_in.UV);
    //FragColor = vec4(abs(fs_in.Normal),1.0);
   // FragColor = vec4(UV,0.0,1.0);
}
