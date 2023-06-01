#version 330                          
out vec4 FragColor;

uniform vec3 _Color;

in Vertex{
    vec3 WorldPos;
    vec3 ViewPos;
    vec3 Normal;
    vec2 UV;
}fs_in;

const int NUM_TEXTURE_TILES = 3;
uniform float _TextureHeights[NUM_TEXTURE_TILES];
uniform sampler2D _Textures[NUM_TEXTURE_TILES];

uniform float _Amplitude;
uniform float _TextureTiling = 1.0;
uniform vec3 _CameraPos;
uniform float _FogDensity = 0.01;
uniform vec3 _FogColor = vec3(0.5,0.6,0.7);

vec3 getTextureColor(vec3 worldPos){
    float height = worldPos.y;
    vec2 worldUV = vec2(fs_in.WorldPos.x,fs_in.WorldPos.z) * _TextureTiling;
    for(int i = 0; i < NUM_TEXTURE_TILES; i++){
        if (height < _TextureHeights[i])
            return texture(_Textures[i],worldUV).rgb;
        else if (height < _TextureHeights[i+1]){
            vec3 colA = texture(_Textures[i],worldUV).rgb;
            vec3 colB = texture(_Textures[i+1],worldUV).rgb;
            float delta = _TextureHeights[i+1] - _TextureHeights[i];
            float t = (height - _TextureHeights[i])/delta;
            return mix(colA,colB,t);
        }
    }
    return texture(_Textures[NUM_TEXTURE_TILES-1],worldUV).rgb;
}

void main(){         
    vec2 worldUV = vec2(fs_in.WorldPos.x,fs_in.WorldPos.z) * _TextureTiling;
    float h = fs_in.WorldPos.y / _Amplitude;
    vec3 col = getTextureColor(fs_in.WorldPos);
    float fogAmount = 1.0 - exp(fs_in.ViewPos.z * _FogDensity);
    col = mix(col,_FogColor,fogAmount);
    FragColor = vec4(col,1);
}
