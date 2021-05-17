#version 460 core

layout (location=0) in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    //vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(1.0,0.0,0.0,1.0);
    //FragColor=vec4(col,1.0);
} 