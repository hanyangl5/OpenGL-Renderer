#version 460 core

layout(location = 0) in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D color_texture;
uniform sampler2D pos_tex;
uniform sampler2D normal_tex;

uniform float ssao_factor;

#define sample_point 32
#define BLOCKER_SEARCH_NUM_SAMPLES sample_point
#define PCF_NUM_SAMPLES sample_point
#define NUM_RINGS 10
float occlusion = 0.0;
vec2 poissonDisk[sample_point];
#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586

#define LIGHT_WIDTH 100

#define LIGHT_WORLD_SIZE .5
#define LIGHT_FRUSTUM_WIDTH 3.75
// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
#define LIGHT_SIZE_UV (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)
highp float rand_1to1(highp float x) {
  // -1 -1
  return fract(sin(x) * 10000.0);
}

highp float rand_2to1(vec2 uv) {
  // 0 - 1
  const highp float a = 12.9898, b = 78.233, c = 43758.5453;
  highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
  return fract(sin(sn) * c);
}
vec2 random2(vec2 st) {
  st = vec2(dot(st, vec2(127.1, 311.7)), dot(st, vec2(269.5, 183.3)));
  return -1.0 + 2.0 * fract(sin(st) * 43758.5453123);
}
float random(float x) {
  float y = fract(sin(x) * 100000.0);
  return y;
}
void poissonDiskSamples(const in vec2 randomSeed) {

  float ANGLE_STEP = PI2 * float(NUM_RINGS) / float(sample_point);
  float INV_NUM_SAMPLES = 1.0 / float(sample_point);

  float angle = rand_2to1(randomSeed) * PI2;
  float radius = INV_NUM_SAMPLES;
  float radiusStep = radius;

  for (int i = 0; i < sample_point; i++) {
    poissonDisk[i] =
        ssao_factor * vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}
void main() {
  float depth = texture(normal_tex, TexCoords).a;
  // float depth = texture(pos_tex, TexCoords).z;
  poissonDiskSamples(TexCoords);
  for (int i = 0; i < sample_point; i++) {
    float sampleDepth = texture(normal_tex, TexCoords + poissonDisk[i]).a;
    if (sampleDepth > depth) {
      occlusion += 1.0;
    }
  }
  // the more occlusion the less visibility
  float visibility = 1.0 - occlusion / float(sample_point);

  vec3 col = texture(color_texture, TexCoords).rgb;
  FragColor = vec4(visibility * col, 1.0);
}