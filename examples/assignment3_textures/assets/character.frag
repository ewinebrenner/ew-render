#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _Texture;
uniform vec4 _Color;
void main(){
	vec4 color = texture(_Texture,UV) * _Color;
	FragColor = color;
}