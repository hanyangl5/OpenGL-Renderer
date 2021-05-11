#version 460 core
out vec4 FragColor;

layout (location=0) in vec2 TexCoords;
layout (location=1) in vec3 view_normal;
uniform sampler2D texture_diffuse1;

void main()
{    
    vec4 texcolor=texture(texture_diffuse1, TexCoords);
    FragColor = texcolor;
}