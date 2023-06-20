#version 450
out vec4 FragColor;
in vec2 UV;
uniform float uTime;
uniform sampler2D uTexture;

void main(){
	vec3 col = texture(uTexture,UV).rgb;
	FragColor = vec4(col,1.0);
}