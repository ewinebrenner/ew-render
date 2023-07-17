#version 450
out vec4 FragColor;
in vec2 UV;

uniform float iTime;
uniform vec2 iResolution;

float sunRadius = 0.3;
vec3 sunColor = vec3(1.0,0.75,0.0);
vec3 buildingCol = vec3(0.2,0.1,0.1);

void main(){
	vec2 uv = UV;
	float aspectRatio = iResolution.x / iResolution.y;
    vec3 bgColorNight = mix(vec3(0.4,0.1,0.2),vec3(0.1,0.2,0.2),uv.y);
    vec3 bgColorDay = mix(vec3(1.0,0.3,0.1),vec3(0.5,0.1,0.2),uv.y);
    float sunT = sin(iTime)*0.5 + 0.5;
    vec3 col = mix(bgColorNight,bgColorDay,sunT);
    
    //Draw sun
    uv.x *= aspectRatio;
    float sunY = sunT - 0.5;
    vec2 center = vec2(0.5 * aspectRatio,sunY);
    sunColor.gb *= sunT;
    float sunD = smoothstep(sunRadius-0.1,sunRadius,length(uv - center));
    col = mix(sunColor.rgb,col,sunD);
    
    //Cool buildings
    float buildingT = mod(floor(uv.x*10.0),2.0);
    float buildingHeight = (sin(uv.x*13.0)*0.5+0.5)*0.2;
    buildingHeight = floor(buildingHeight * 15.0)*0.1+0.1;
    buildingT *= step(uv.y,buildingHeight);
    
    float sunGlow = 1.0 - smoothstep(0.0,sunRadius*2.0,length(uv - center));
    vec3 bCol = mix(buildingCol,buildingCol + sunColor * 0.2,sunGlow);
    
    col = mix(col,bCol,buildingT);
    // Output to screen
    FragColor = vec4(col,1.0);
}