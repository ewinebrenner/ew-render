#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec4 vPosition;
layout(location = 2) in vec4 vColor;

out vec4 Color;

uniform mat4 _View;
uniform mat4 _Projection;
uniform int _Billboard;

void main(){
	vec3 worldPos;

    if (_Billboard == 1){
		//Billboarded worldspace positions
		vec3 camRight = vec3(_View[0][0],_View[1][0],_View[2][0]);
		vec3 camUp = vec3(_View[0][1],_View[1][1],_View[2][1]);

		worldPos = vPosition.xyz 
		+ camRight * vPos.x * vPosition.w
		+ camUp * vPos.y * vPosition.w;
	}else{
		worldPos = vPosition.xyz + vPos * vPosition.w;
	}

	gl_Position = _Projection * _View * vec4(worldPos,1.0);

	Color = vColor;
}