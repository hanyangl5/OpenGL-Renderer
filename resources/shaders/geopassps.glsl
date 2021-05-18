#version 460 core

layout (location=0) in PS_IN{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
}ps_in;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;



uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_roughness1;
void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition.rgb = ps_in.FragPos;
    //gPosition.a=
    // [normalx,normaly,normalz,roughness]
    gNormal.rgb = normalize(ps_in.Normal);
    //gNormal.a=
    // [albedor,albedog,albedob,specular]
    gAlbedoSpec.rgb = texture(texture_diffuse1, ps_in.TexCoords).rgb;
    gAlbedoSpec.a = texture(texture_specular1, ps_in.TexCoords).r;
}