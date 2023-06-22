#version 450
out vec4 FragColor;
in vec2 UV;
uniform float uTime;
uniform sampler2D uTexture;
uniform float uScale = 2.0f;

void main(){
	vec2 uv = UV;
	uv*=uScale;
	vec3 col = texture(uTexture,uv).rgb;
	FragColor = vec4(col,1.0);
}