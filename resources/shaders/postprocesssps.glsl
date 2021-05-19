#version 460 core

layout(location = 0) in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;

void main() {
  vec3 col = texture(screenTexture, TexCoords).rgb;
  col = col / (col + vec3(1.0));
  col = pow(col, vec3(1.0 / 2.2));
  FragColor = vec4(col, 1.0);
}