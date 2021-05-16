#version 460 core

layout (location=0) in PS_IN{
    vec2 texcoord;
}ps_in;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, ps_in.texcoord).rgb;
    vec3 red=vec3(1.0,0.0,0.0);
    gl_FragColor = vec4(red, 1.0);
} 