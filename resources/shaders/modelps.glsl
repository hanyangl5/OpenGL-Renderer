#version 460 core

layout (location=0) in PS_IN{
    vec2 texcoord;
    vec3 world_normal;
}ps_in;

layout (std140) uniform Light
{
    vec3 pos;
    vec3 dir;
    vec3 color;
    int type;
}light[];
uniform vec3 eyepos;
uniform sampler2D texture_diffuse1;
//uniform sampler2D shadowtexture;


void main()
{
    vec3 world_normal=normalize(ps_in.world_normal);
    vec3 light_dir=normalize(-light[0].dir);
    float kd=max(0,dot(world_normal,light_dir));
    //vec3 texcolor=texture(texture_diffuse1, ps_in.texcoord);
    //float d=texture(shadowtexture,ps_in.texcoord).r;
    //gl_FragColor = vec4(d,d,d,1.0);
    gl_FragColor=texture(texture_diffuse1,ps_in.texcoord);

}