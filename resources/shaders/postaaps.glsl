#version 460 core

layout(location = 0) in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float luma_threshold;

vec4 fxaa(sampler2D tex, vec2 fragCoord);

void main() { FragColor = fxaa(screenTexture, TexCoords); }

vec4 fxaa(sampler2D tex, vec2 fragCoord) {
  vec4 color;
  // sample neighbour texture color
  vec3 rgbNW = texture2D(tex, (fragCoord + vec2(-1, 1))).rgb;
  vec3 rgbNE = texture2D(tex, (fragCoord + vec2(1, 1))).rgb;
  vec3 rgbSW = texture2D(tex, (fragCoord + vec2(-1, -1))).rgb;
  vec3 rgbSE = texture2D(tex, (fragCoord + vec2(1, -1))).rgb;

  vec3 rgbM = texture2D(tex, fragCoord).rgb;
  // calculate luminance
  vec3 luma = vec3(0.299, 0.587, 0.114);
  float lumaNW = dot(rgbNW, luma);
  float lumaNE = dot(rgbNE, luma);
  float lumaSW = dot(rgbSW, luma);
  float lumaSE = dot(rgbSE, luma);
  float lumaM = dot(rgbM, luma);

  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
  float lumaContrast = lumaMax - lumaMin;
  return vec4(lumaContrast, lumaContrast, lumaContrast, 1.0);
  // no aa
  // if (lumaMax - lumaMin <= lumaMax * luma_threshold 0) {
  //   color = vec4(rgbM, 1.0);
  //   return color;
  // } else
  //   return vec4(1.0, 1.0, 1.0, 1.0);
}