#version 330                          
layout (location = 0) in vec3 in_Pos;  
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_UV;

out Vertex{
    vec3 WorldPos;
    vec3 ViewPos;
    vec3 Normal;
    vec2 UV;
}vs_out;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;
uniform sampler2D _HeightMap;
uniform float _Frequency;
uniform float _Amplitude;

float evaluateHeight(vec2 uv){
    float h = texture(_HeightMap,uv * _Frequency).r * _Amplitude;
    h -= texture(_HeightMap,uv * _Frequency * 2.0).r * _Amplitude * 0.5;
    h -= texture(_HeightMap,uv * _Frequency * 8.0).r * _Amplitude * 0.1;
    return h;
}

void main(){      
    //Apply height offsets in local space
    vec3 localPos = in_Pos;
    localPos.y = evaluateHeight(in_UV);

    vs_out.WorldPos = vec3(_Model * vec4(localPos,1));
    vs_out.Normal = in_Normal;
    vs_out.UV = in_UV;
    vs_out.ViewPos = vec3(_View * vec4(vs_out.WorldPos,1));
    gl_Position = _Projection * _View * vec4(vs_out.WorldPos,1);
}
