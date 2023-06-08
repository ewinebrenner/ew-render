#version 330                          
out vec4 FragColor;


in Vertex{
    vec2 UV;
}fs_in;

uniform sampler2D _FontTexture;

uniform int _DEBUG = 0;

uniform vec4 _Color;
uniform vec3 _OutlineColor;
uniform float _Time;
uniform float _Thickness = 0.5;
uniform float _OutlineThickness = 0.1;

void main(){         
    float a = texture(_FontTexture, fs_in.UV).r;
    if (_DEBUG == 1){
       FragColor = vec4(a,a,a,1.0);
       return;
    }
    float baseDistance = 1.0-_Thickness;
    float alpha = smoothstep(baseDistance-0.02,baseDistance+0.02,a);

    vec3 col = _Color.rgb;
    col = mix(_OutlineColor,col,smoothstep(baseDistance+_OutlineThickness-0.01,baseDistance+_OutlineThickness+0.01,a));
   // col.gb += cos(_Time*2.0);
    FragColor = vec4(col,_Color.a * alpha);
}
