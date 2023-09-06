#version 450
out vec4 FragColor;

in Surface{
	vec2 UV;
	vec4 WorldPosition;
	vec3 WorldNormal;
}fs_in;

uniform sampler2D _Texture;

uniform struct Light{
	vec3 position;
	vec4 color;
}_Light;

uniform struct Material{
	float ambientK;
	float diffuseK;
	float specularK;
	float shininess;
}_Material;

uniform vec3 _EyePos;

void main(){
	vec3 normal = normalize(fs_in.WorldNormal);

	float ambient = _Material.ambientK;
	vec3 toLight = normalize(_Light.position - fs_in.WorldPosition.xyz);
	float diffuse = max(dot(toLight,normal),0.0) * _Material.diffuseK;
	vec3 toEye = normalize(_EyePos - fs_in.WorldPosition.xyz);
	vec3 halfVector = normalize(toLight + toEye);
	float specular = pow(max(dot(halfVector,normal),0.0),_Material.shininess) * _Material.specularK;
	vec3 lightColor = (ambient + diffuse + specular) * _Light.color.rgb;
	//vec3 color = texture(_Texture,fs_in.UV).rgb * lightColor;
	vec3 color = lightColor;
	FragColor = vec4(mix(abs(fs_in.WorldNormal),color,0.8),1.0);
	//FragColor = vec4(abs(fs_in.WorldNormal),1.0);
}