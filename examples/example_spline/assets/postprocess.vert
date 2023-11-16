#version 450

vec2 positions[4]={
	vec2(-1.0,-1.0),
	vec2(1.0,-1.0),
	vec2(1.0,1.0),
	vec2(-1.0,1.0)
};
const int indices[6] = int[6](0,1,2,2,3,0);

out vec2 UV;
void main(){
	vec2 pos = positions[indices[gl_VertexID]].xy;
	UV = pos * 0.5 + 0.5;
	gl_Position = vec4(pos,0,1);
}