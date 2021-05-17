#version 460 core

layout (location=0) in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;

    float gamma=2.2;
    FragColor=vec4(pow(col.rgb,vec3(1.0/gamma)),1.0);
} 