#version 450
out vec4 FragColor;
in vec4 Color;
in vec2 UV;
void main(){
	vec2 uv = UV;
	float a = step(distance(uv,vec2(0.5)),0.5);
	if (a < 0.5)
		discard;
	FragColor = vec4(Color.rgba);
	//FragColor = Color;
}