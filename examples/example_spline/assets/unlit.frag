#version 450
layout(location = 0) out vec4 FragColor;
layout(location = 1) out uvec4 ObjectIDColor;
in Surface{
	vec2 UV;
	vec3 Normal;
}fs_in;

uniform vec4 _Color;
uniform int _ObjectIndex;
void main(){
	FragColor = _Color;
	ObjectIDColor = uvec4(_ObjectIndex,0,0,1);
}