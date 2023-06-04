#version 450
out vec4 FragColor;
in vec3 ViewPos;
in vec3 WorldPos;

uniform vec3 _BottomColor = vec3(0.6,0.8,1.0);
uniform vec3 _TopColor = vec3(0.3,0.4,0.8);
uniform vec3 _LightDir = vec3(0.0,-1.0,0.0);
uniform float _SunSize = 512;
vec3 getSun(vec3 fragDir,vec3 lightDir, float powExp){
	
	float sun = clamp(dot(lightDir,fragDir),0.0,1.0);
	vec3 col = 0.8 * vec3(1.0,0.6,0.1)*pow(sun,powExp);
	return col;
}

void main(){
    float height = max(WorldPos.y,0.0);
	vec3 lightDir = _LightDir;
	lightDir.y *=-1;
	vec3 fragDir = normalize(WorldPos);
	//vec3 bgColor = vec3(0.6,0.71,0.85) - lightDir.y*0.2*vec3(1.0,0.5,1.0) + 0.15*0.5;
	vec3 bgColor = mix(_BottomColor,_TopColor,height);
	bgColor+=getSun(fragDir,lightDir,_SunSize);
	FragColor = vec4(bgColor,1.0);
}