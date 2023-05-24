#version 330                          
out vec4 FragColor;

uniform vec3 _Color;
in vec3 Normal;

void main(){         
    FragColor = vec4(abs(Normal),1.0f);
}
