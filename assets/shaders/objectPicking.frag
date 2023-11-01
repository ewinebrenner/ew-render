#version 450
out uvec3 FragColor;
uniform int _ObjectIndex;

void main(){
	FragColor = uvec3(_ObjectIndex,0,0);
}