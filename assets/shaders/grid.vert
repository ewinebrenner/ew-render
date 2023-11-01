#version 450
layout(location=0) out vec2 uv;
const vec3 pos[4] = vec3[4](
	vec3(-1.0,0.0,-1.0),
	vec3(1.0,0.0,-1.0),
	vec3(1.0,0.0,1.0),
	vec3(-1.0,0.0,1.0)
);
const int indices[6] = int[6](0,2,1,3,2,0);

float gridSize = 100.0;

uniform mat4 _ViewProjection;

void main(){
	vec3 vPos = pos[indices[gl_VertexID]] * gridSize;
	gl_Position = _ViewProjection * vec4(vPos,1.0);
	uv = vPos.xz;
}