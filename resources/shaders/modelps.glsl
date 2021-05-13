#version 460 core
out vec4 FragColor;

//layout (location=0) in vec2 TexCoords;
//layout (location=1) in vec3 view_normal;

layout (location=0) in PS_IN{
    vec2 texcoord;
    vec3 view_normal;
}ps_in;

uniform sampler2D texture_diffuse1;

void main()
{

    vec4 texcolor=texture(texture_diffuse1, ps_in.texcoord);
    FragColor = texcolor;
}