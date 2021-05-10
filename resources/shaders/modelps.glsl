#version 460 core
out vec4 FragColor;

layout (location=0) in vec2 TexCoords;
layout (location=1) in vec3 view_normal;
uniform sampler2D texture_diffuse1;

void main()
{    
    vec3 z_dir=vec3(0.0,0.0,1.0);
    vec4 texcolor=texture(texture_diffuse1, TexCoords);
    FragColor = vec4(view_normal,1.0);
}