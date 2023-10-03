#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 Normal;
}fs_in;

void main(){
	FragColor = vec4(abs(fs_in.Normal),1.0);
}