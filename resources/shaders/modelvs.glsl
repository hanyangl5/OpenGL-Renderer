#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location=0) out vec2 TexCoords;
layout (location=1) out vec3 view_normal;

uniform mat4 model1;
uniform mat4 view;
uniform mat4 projection;

// layout (std140) uniform Light
// {
//     vec3 pos;
//     vec3 dir;
//     vec3 color;
//     int type;
// };

void main()
{
    TexCoords = aTexCoords; 
    view_normal=vec3( model1 * vec4(aNormal, 1.0));
    gl_Position = projection * view * model1 * vec4(aPos, 1.0);
}