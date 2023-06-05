#version 330                          
out vec4 FragColor;

uniform vec3 _Color;

in Vertex{
    vec3 WorldPos;
    vec3 ViewPos;
    vec3 Normal;
    vec2 UV;
    vec3 BaryCoords;
}fs_in;

const int NUM_TEXTURE_TILES = 3;
uniform float _TextureHeights[NUM_TEXTURE_TILES];
uniform sampler2D _Textures[NUM_TEXTURE_TILES];

uniform float _Amplitude;
uniform float _TextureTiling = 1.0;
uniform vec3 _CameraPos;
uniform float _FogDensity = 0.01;
uniform vec3 _FogColor = vec3(0.5,0.6,0.7);
uniform float _Time;
uniform struct Light{
    vec3 direction;
    vec3 diffuseColor;
    vec3 ambientColor;
}_Light;

vec3 getTextureColor(vec3 worldPos){
    float height = worldPos.y;
    vec2 uv = fs_in.UV * _TextureTiling;
    for(int i = 0; i < NUM_TEXTURE_TILES; i++){
        if (height < _TextureHeights[i])
            return texture(_Textures[i],uv).rgb;
        else if (height < _TextureHeights[i+1]){
            vec3 colA = texture(_Textures[i],uv).rgb;
            vec3 colB = texture(_Textures[i+1],uv).rgb;
            float delta = _TextureHeights[i+1] - _TextureHeights[i];
            float t = (height - _TextureHeights[i])/delta;
            return mix(colA,colB,t);
        }
    }
    return texture(_Textures[NUM_TEXTURE_TILES-1],uv).rgb;
}

vec3 applyFog(vec3 col, vec3 toCamera, float fogDensity){
    float sunAmnt = max(dot(-toCamera,_Light.direction),0.0);
    sunAmnt = pow(sunAmnt,8.0);
    //Warm tint when facing toward camera
    vec3 fogColor = mix(vec3(0.5,0.6,0.7),vec3(1.0,0.9,0.7),sunAmnt);
    float fogAmount = 1.0 - exp(fs_in.ViewPos.z * fogDensity);
    col = mix(col,fogColor,fogAmount);
    return col;
}

//Returns 0 or 1 based on closeness to edge, where 1 = edge, 0 = not edge
//Thickness is in screen-space pixels
float edgeFactor(float thickness)
{
    //fwidth returns sum of absolute value of derivatives in x and y
	vec3 a3 = smoothstep( vec3( 0.0 ), fwidth(fs_in.BaryCoords) * thickness, fs_in.BaryCoords);
	return min( min( a3.x, a3.y ), a3.z );
}

void main(){         
    vec3 albedo = getTextureColor(fs_in.WorldPos);
    vec3 toCamera = normalize(_CameraPos - fs_in.WorldPos);
    
    //Lighting
    vec3 normal = normalize(fs_in.Normal);

    float diffuseFactor = max(dot(normal,-_Light.direction),0.0);
    vec3 light = _Light.diffuseColor * diffuseFactor + _Light.ambientColor;

    vec3 col = albedo * light;

    col = applyFog(col,toCamera,_FogDensity);

    //Wireframe render
    col = mix(col*0.1,col,edgeFactor(1.0));
    FragColor = vec4(col,1);
}
