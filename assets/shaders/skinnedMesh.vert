#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;
layout(location = 4) in ivec4 vBoneIDs; 
layout(location = 5) in vec4 vWeights;
	
uniform mat4 _ViewProjection;
uniform mat4 _Model;
	
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 _FinalBoneMatrices[MAX_BONES];

out Surface{
	vec2 UV;
	vec4 WorldPosition;
	vec3 WorldNormal;
}vs_out;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(vBoneIDs[i] == -1) 
            continue;
        if(vBoneIDs[i] >=MAX_BONES) 
        {
            totalPosition = vec4(vPos,1.0f);
            break;
        }
        vec4 localPosition = _FinalBoneMatrices[vBoneIDs[i]] * vec4(vPos,1.0f);
        totalPosition += localPosition * vWeights[i];
        vec3 localNormal = mat3(_FinalBoneMatrices[vBoneIDs[i]]) * vNormal;
    }
    vec4 worldPosition = _Model * totalPosition;
    vs_out.UV = vUV;
    vs_out.WorldPosition = worldPosition;
    vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;

    gl_Position = _ViewProjection * worldPosition;
}