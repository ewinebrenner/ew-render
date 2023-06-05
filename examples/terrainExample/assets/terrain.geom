//Geometry shader that adds barycentric coordinates to each vertex
//Based on 3D Graphics Rendering Cookbook Ch2_Sample10_MeshOptimizer https://github.com/PacktPublishing/3D-Graphics-Rendering-Cookbook
#version 450 core

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in Vertex{
    vec3 WorldPos;
    vec3 ViewPos;
    vec3 WorldNormal;
    vec2 UV;
}geom_in[];

out Vertex{
	vec3 WorldPos;
    vec3 ViewPos;
    vec3 WorldNormal;
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
		geo_out.WorldPos = geom_in[i].WorldPos;
		geo_out.ViewPos = geom_in[i].ViewPos;
		geo_out.WorldNormal = geom_in[i].WorldNormal;
		geo_out.UV = geom_in[i].UV;
		geo_out.BaryCoords = bc[i];
		
		EmitVertex();
	}
	EndPrimitive();
}