#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 Normal;
}fs_in;

uniform vec4 _Color;

void main(){
	FragColor = _Color;
}