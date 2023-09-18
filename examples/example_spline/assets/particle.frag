#version 450
out vec4 FragColor;
in vec4 Color;
in vec2 UV;
void main(){
	vec2 uv = UV;
	float a = 1.0-smoothstep(0.4,0.5,distance(uv,vec2(0.5)));
	FragColor = vec4(Color.rgb,a * Color.a);
	//FragColor = Color;
}