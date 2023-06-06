#version 450 core

layout(triangles, fractional_even_spacing, ccw) in;
//layout(quads, fractional_odd_spacing, ccw) in;

//Data of 3 vertices of this triangle
in Vertex{
    vec2 UV;
}te_in[];

//Data for each output vertex
out Vertex{
    vec3 WorldPos;
    vec3 ViewPos;
    vec3 WorldNormal;
    vec2 UV;
}te_out;

uniform mat4 _Model;
uniform mat4 _View;
uniform mat4 _Projection;

uniform sampler2D _HeightMap;
uniform float _Frequency;
uniform float _Amplitude;

float evaluateHeight(vec2 uv){ 
    return texture(_HeightMap,uv * _Frequency).r * _Amplitude;
}

vec4 interpVec4(vec4 v0, vec4 v1, vec4 v2){
    return (gl_TessCoord.x * v0) + (gl_TessCoord.y * v1) + (gl_TessCoord.z * v2);
}
vec2 interpVec2(vec2 v0, vec2 v1, vec2 v2){
    return (gl_TessCoord.x * v0) + (gl_TessCoord.y * v1) + (gl_TessCoord.z * v2);
}

void main(){
   //Barycentric interpolation
    vec4 pos = interpVec4(gl_in[0].gl_Position,gl_in[1].gl_Position,gl_in[2].gl_Position);
    te_out.UV = interpVec2(te_in[0].UV,te_in[1].UV,te_in[2].UV);

    pos.y = evaluateHeight(te_out.UV);

    float hr = evaluateHeight(te_out.UV + vec2(0.01,0.0));
    float hl = evaluateHeight(te_out.UV - vec2(0.01,0.0));
    float ht = evaluateHeight(te_out.UV + vec2(0.0,0.01));
    float hb = evaluateHeight(te_out.UV - vec2(0.0,0.01));

    vec3 normal = normalize(vec3(hr-hl,4,ht-hb));
    vec4 worldPos = _Model * pos;
    vec4 viewPos = _View * worldPos;

    te_out.WorldPos = vec3(worldPos);
    te_out.WorldNormal = inverse(transpose(mat3(_Model))) * normal;
    te_out.ViewPos = vec3(viewPos);

    gl_Position = _Projection * viewPos;
}