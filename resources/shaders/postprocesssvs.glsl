#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

layout (location=0) out PS_IN{
    vec2 texcoord;
}vs_out;

void main()
{
    vs_out.texcoord = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
}  