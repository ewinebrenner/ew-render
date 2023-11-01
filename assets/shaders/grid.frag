#version 450
out vec4 FragColor;
layout(location=0) in vec2 uv;

//Log(x) / Log(10)
float log10(float x){
	return log(x) / log(10.0);
}
//Clamps x between 0-1
float satf(float x){
	return clamp(x,0.0,1.0);
}
//Clamps x.x and x.y between 0-1
vec2 satv(vec2 x){
	return clamp(x,vec2(0.0),vec2(1.0));
}
//Returns max of v.x,v.y
float max2(vec2 v){
	return max(v.x,v.y);
}

float gridSize = 100.0;
float gridCellSize = 0.025;
vec4 gridColorThin = vec4(0.5,0.5,0.5,0.3);
vec4 gridColorThick = vec4(0.75,0.75,0.75,0.4);
const float gridMinPixelsBetweenCells = 4.0;

void main(){
	//Screenspace length of UV derivatives
	vec2 dudv = vec2(
		length(vec2(dFdx(uv.x), dFdy(uv.x))),
		length(vec2(dFdx(uv.y),dFdy(uv.y)))
	);
	//Choose LodLevel for this fragment based on pixels between cells.
	float lodLevel = max(0.0,log10((length(dudv)*gridMinPixelsBetweenCells)/gridCellSize)+1.0);

	float lodFade = fract(lodLevel);
	float lod0 = gridCellSize * pow(10.0,floor(lodLevel+0));
	float lod1 = lod0 * 10.0;
	float lod2 = lod1 * 10.0;

	//Increase screen coverage to 4 pixels for anti-aliasing
	dudv*=4.0;

	//Alphas of each LOD - 0 in center of grid squares
	float lod0a = max2(vec2(1.0) - abs(satv(mod(uv,lod0) / dudv) * 2.0 - vec2(1.0)));
	float lod1a = max2(vec2(1.0) - abs(satv(mod(uv,lod1) / dudv) * 2.0 - vec2(1.0)));
	float lod2a = max2(vec2(1.0) - abs(satv(mod(uv,lod2) / dudv) * 2.0 - vec2(1.0)));

	vec4 c = lod2a > 0.0 ? gridColorThick : lod1a > 0.0 ? mix(gridColorThick,gridColorThin,lodFade) : gridColorThin;

	//Fade away - 0 alpha at gridSize extents
	float opacityFalloff = (1.0 - satf(length(uv)/gridSize));
	c.a *= lod2a > 0.0 ? lod2a : lod1a > 0.0 ? lod1a :(lod0a * 1.0 - lodFade);
	c.a *= opacityFalloff;
	FragColor = c;
}