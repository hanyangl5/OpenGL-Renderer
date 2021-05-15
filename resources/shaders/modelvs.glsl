#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location=0) out PS_IN{
    vec2 texcoord;
    vec3 world_normal;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    vs_out.texcoord=aTexCoords;
    vs_out.world_normal=vec3( model * vec4(aNormal, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}