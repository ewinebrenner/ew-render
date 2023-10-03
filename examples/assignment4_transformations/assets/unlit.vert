#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 Normal;
}vs_out;

uniform mat4 _Model;

void main(){
	vs_out.UV = vUV;
	vs_out.Normal = vNormal;
	gl_Position = _Model * vec4(vPos,1.0);
	gl_Position.z *= -1.0;
}