#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec3 Normal;
	vec3 WorldPos;
}fs_in;
uniform int _DebugMode;
uniform sampler2D _Texture;
uniform vec3 _LightDir = vec3(0,1,0);
uniform vec3 _FlatColor = vec3(0);

uniform float _AmbientK = 0.2;
uniform float _DiffuseK = 0.5;
uniform float _SpecularK = 0.5;
uniform float _Shininess = 64;
uniform vec3 _CameraPosition;

void main(){

	vec2 uv = fs_in.UV;
	vec3 normal = normalize(fs_in.Normal);

	if (_DebugMode == 0){
		FragColor = vec4(normal * 0.5 + 0.5,1.0);
	}else if (_DebugMode == 1){
		FragColor = vec4(uv.x,uv.y,0.0,1.0);
	}else if (_DebugMode == 2){
		FragColor = texture(_Texture,uv).rgba;
	}else if (_DebugMode == 3){
		FragColor = texture(_Texture,uv).rgba;
		float light = _AmbientK + _DiffuseK * max(dot(normal,normalize(-_LightDir)),0.0);
		vec3 toEye = normalize(_CameraPosition - fs_in.WorldPos);
		vec3 h = normalize(toEye + -_LightDir);
		light += _SpecularK * pow(max(0.0,dot(normal,h)),_Shininess);
		FragColor.rgb *= light;
	}else if (_DebugMode == 4){
		FragColor.rgb = _FlatColor;
	}
}