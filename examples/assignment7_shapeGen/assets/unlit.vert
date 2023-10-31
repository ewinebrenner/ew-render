#version 450
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface{
	vec2 UV;
	vec3 Normal;
}vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;
uniform vec3 _ClipPlaneOrigin;
uniform vec3 _ClipPlaneNormal;
//#define CLIPPING_PLANE_ENABLED
void main(){
	vs_out.UV = vUV;
	vs_out.Normal = vNormal;

	vec4 worldPos = _Model * vec4(vPos,1.0);
	vec3 p = worldPos.xyz;

	#if CLIPPING_PLANE_ENABLED
	vec3 planeNormal = normalize(_ClipPlaneNormal);
	float d = dot(planeNormal,(p - _ClipPlaneOrigin));
	if (d > 0){
		p = p - planeNormal * d;
		//if (dot(vs_out.Normal,planeNormal)>0)
		vs_out.Normal = planeNormal;
	}
	#endif
	
	gl_Position = _ViewProjection * vec4(p,1.0);
}