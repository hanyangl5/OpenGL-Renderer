#version 460 core

layout (location=0) in vec2 TexCoords;
layout (location=1) in vec3 world_normal;
layout (location=2) in vec3 world_pos;


layout (location=0) out vec4 FragColor;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 texcolor=texture(texture_diffuse1, TexCoords);
    FragColor = texcolor;
}