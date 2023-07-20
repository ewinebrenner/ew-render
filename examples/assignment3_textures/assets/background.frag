#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _TextureA;
uniform sampler2D _TextureB;
uniform sampler2D _NoiseTexture;
uniform float _Time;
uniform float _DistortionStrength;
uniform float _DistortionSpeed;
uniform vec4 _GlowColor;

void main(){
	//float noise = texture(_NoiseTexture,UV+_Time*_DistortionSpeed).r;
	float noise = texture(_NoiseTexture,UV).r;
    vec2 uv = UV + noise * 0.1f;
	vec4 colorA = texture(_TextureA,uv);
	vec4 colorB = texture(_TextureB,uv);
	vec3 color = mix(colorA.rgb,colorB.rgb,colorB.a * 0.5);
	//color += noise * _GlowColor.rgb;
	FragColor = vec4(color,1.0);
}