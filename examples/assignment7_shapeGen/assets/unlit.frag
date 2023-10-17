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
	vec3 normal = normalize(fs_in.Normal);

	if (_DebugMode == 0){
		FragColor = vec4(abs(normal),1.0);
	}else if (_DebugMode == 1){
		FragColor = vec4(uv.x,uv.y,0.0,1.0);
	}else if (_DebugMode > 1){
		FragColor = texture(_Texture,uv).rgba;
		if (_DebugMode == 3)
			FragColor.rgb *= 0.3 + 0.7 * max(dot(normal,normalize(-_LightDir)),0.0);
	}
}