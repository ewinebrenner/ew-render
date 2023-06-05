//Geometry shader that adds barycentric coordinates to each vertex
//Based on 3D Graphics Rendering Cookbook Ch2_Sample10_MeshOptimizer https://github.com/PacktPublishing/3D-Graphics-Rendering-Cookbook
#version 450

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in Vertex{
    vec3 WorldPos;
    vec3 ViewPos;
    vec3 Normal;
    vec2 UV;
}vs_in[];

out Vertex{
	vec3 WorldPos;
    vec3 ViewPos;
    vec3 Normal;
    vec2 UV;
	vec3 BaryCoords;
}geo_out;

void main()
{
	const vec3 bc[3] = vec3[]
	(
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);
	for ( int i = 0; i < 3; i++ )
	{
		gl_Position = gl_in[i].gl_Position;

		//Copy vertex data input to output
		geo_out.WorldPos = vs_in[i].WorldPos;
		geo_out.ViewPos = vs_in[i].ViewPos;
		geo_out.Normal = vs_in[i].Normal;
		geo_out.UV = vs_in[i].UV;
		geo_out.BaryCoords = bc[i];
		
		EmitVertex();
	}
	EndPrimitive();
}