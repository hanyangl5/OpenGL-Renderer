#version 460 core
in vec3 TexCoords;
out vec4 FragColor;

uniform samplerCube skybox;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;
uniform int WIDTH, HEIGHT;
void main() {

  vec2 uv = vec2(float(gl_FragCoord.x) / float(WIDTH),
                 float(gl_FragCoord.y) / float(HEIGHT));
  float depth = texture(normal_tex, uv).a;

  if (depth == 1.0) {
    FragColor = texture(skybox, TexCoords);
  } else {
    FragColor = texture(color_tex, uv);
  }
}