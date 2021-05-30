#version 460 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform sampler2D normal_tex;
void main() {
  FragColor = texture(skybox, TexCoords);
  vec2 uv = vec2(gl_FragCoord.x / 800, gl_FragCoord.y / 600);
  float depth = texture(normal_tex, uv).a;
  FragColor = vec4(depth, depth, depth, 1.0);
  // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}