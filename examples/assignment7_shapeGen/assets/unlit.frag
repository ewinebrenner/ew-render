#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 Normal;
}fs_in;
uniform int _DebugMode;
uniform sampler2D _Texture;
uniform vec3 _LightDir = vec3(0,1,0);

void main(){
	vec2 uv = fs_in.UV;
	if (_DebugMode == 0){
		FragColor = vec4(abs(fs_in.Normal),1.0);
	}else if (_DebugMode == 1){
		FragColor = vec4(uv.x,uv.y,0.0,1.0);
	}else if (_DebugMode == 2){
		FragColor = texture(_Texture,fs_in.UV).rgba;
		FragColor.rgb *= max(dot(fs_in.Normal,normalize(-_LightDir)),0.0);
	}
}