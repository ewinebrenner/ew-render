#version 450 core
out vec4 FragColor;

in Vertex{
    vec2 UV;
	vec3 WorldPos;
	vec3 WorldNormal;
}fs_in;

void main(){
	//FragColor = vec4(0.0,0.0,0.0,1.0);
	//FragColor = vec4(fs_in.UV.x,fs_in.UV.y,0.0,1.0);
	vec3 normal = normalize(fs_in.WorldNormal);
	//vec3 normal = normalize(cross(dFdx(fs_in.WorldPos),dFdy(fs_in.WorldPos)));
	FragColor = vec4(normal,1.0);
}