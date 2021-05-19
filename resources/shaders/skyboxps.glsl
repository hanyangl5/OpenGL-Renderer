#version 460 core

in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform sampler2D pos;
void main() {
  float d = texture(pos, TexCoords.xy).b;
  if (d < 1.0) {
    FragColor = texture(skybox, TexCoords);
  } else {
    FragColor = vec4(1.0, 1.0, 1.0, 0.0);
  }
}