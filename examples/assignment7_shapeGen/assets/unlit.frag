#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 Normal;
}fs_in;

void main(){
	vec2 uv = fs_in.UV;
	FragColor = vec4(abs(fs_in.Normal),1.0);
	//FragColor = vec4(uv.x,uv.y,0.0,1.0);
}