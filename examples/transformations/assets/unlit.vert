#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
out vec2 UV;
uniform float uTime;
void main(){
	UV = vUV;
	vec3 offset = vec3(0,sin(vPos.x + uTime),0)*0.1;
	gl_Position = vec4(vPos + offset,1.0);
}