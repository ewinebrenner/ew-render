#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _TextureA;
uniform sampler2D _TextureB;

void main(){
	vec4 colorA = texture(_TextureA,UV);
	vec4 colorB = texture(_TextureB,UV);
	vec3 color = mix(colorA.rgb,colorB.rgb,colorB.a);
	FragColor = vec4(color,1.0);
}