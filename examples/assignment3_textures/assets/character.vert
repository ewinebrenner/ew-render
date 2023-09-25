#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
uniform float _Time;
uniform float _Speed;
void main(){
	UV = vUV;
	vec3 pos = vPos * 0.5;
	float time = _Time*_Speed;
	pos.x+=sin(time)*0.2;
	pos.y+=(abs(cos(time))*0.5-0.5);
	gl_Position = vec4(pos,1.0);
}