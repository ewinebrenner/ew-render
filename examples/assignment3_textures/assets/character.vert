#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
uniform float _Time;
void main(){
	UV = vUV;
	vec3 pos = vPos;
	pos.x+=sin(_Time*4.0)*0.2;
	pos.y+=abs(cos(_Time*4.0))*0.5-0.5;
	gl_Position = vec4(pos,1.0);
}