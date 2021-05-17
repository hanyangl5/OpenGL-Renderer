#version 460 core

layout (location=0) in PS_IN{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
}ps_in;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;



uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = ps_in.FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(ps_in.Normal);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse1, ps_in.TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular1, ps_in.TexCoords).r;
}