#version 460 core

layout(location = 0) in PS_IN {
  vec3 FragPos;
  vec2 TexCoords;
  vec4 Normal;
}
ps_in;

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out vec4 gMetallicRoughness;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_metallicroughness1;

void main() {
  // store the fragment position vector in the first gbuffer texture
  gPosition.rgb = ps_in.FragPos;
  // [normalx,normaly,normalz,depth]
  gNormal = ps_in.Normal;
  // [albedo]
  gAlbedo.rgb = texture(texture_diffuse1, ps_in.TexCoords).rgb;
  // metallic roughness
  gMetallicRoughness.rg =
      texture(texture_metallicroughness1, ps_in.TexCoords).rg;
}