#version 450
out vec4 FragColor;

in vec3 WorldPos;

uniform sampler2D _NoiseTex;
uniform vec4 _Color;
uniform float _Time;
void main(){
	vec2 uv = vec2(WorldPos.x,WorldPos.z) / 200 + _Time * 0.05;
	float noise = texture(_NoiseTex,uv).r;
	//float alpha = 1.0 - smoothstep(0.3,0.5,noise);
	float alpha = noise;

	FragColor = vec4(_Color.rgb,_Color.a * alpha);
}