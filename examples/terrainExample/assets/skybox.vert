#version 450

layout (location = 0) in vec3 in_Pos;  

uniform mat4 _Model = mat4(1);
uniform mat4 _View = mat4(1);
uniform mat4 _Projection;

out vec3 ViewPos;
out vec3 WorldPos;
void main(){
	vec4 worldPos = _Model * vec4(in_Pos,1.0);
	vec4 viewPos = _View * worldPos;
	vec4 pos = _Projection * viewPos;
	gl_Position = pos.xyww;

	ViewPos = vec3(viewPos);
	WorldPos = vec3(worldPos);
}