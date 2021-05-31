#version 460 core

layout(location = 0) in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float luma_threshold;
uniform int WIDTH, HEIGHT;

vec4 fxaa(sampler2D tex, vec2 fragCoord) {
  vec4 color;
  // sample neighbour texture color
  vec3 rgbNW =
      texture2D(tex, fragCoord + vec2(-1.0 / float(WIDTH), 1.0 / float(HEIGHT)))
          .rgb;
  vec3 rgbNE =
      texture2D(tex, fragCoord + vec2(1.0 / float(WIDTH), 1.0 / float(HEIGHT)))
          .rgb;
  vec3 rgbSW = texture2D(tex, fragCoord + vec2(-1.0 / float(WIDTH),
                                               -1.0 / float(HEIGHT)))
                   .rgb;
  vec3 rgbSE =
      texture2D(tex, fragCoord + vec2(1.0 / float(WIDTH), -1.0 / float(HEIGHT)))
          .rgb;

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
  // skipping low contrast pixels
  if (lumaContrast < lumaMax * luma_threshold) {
    return vec4(rgbM, 1.0);
  }
  // calculating blending factor
  return vec4(0.2 * (rgbNE + rgbNE + rgbSW + rgbSE + rgbM), 1.0);

  // return vec4(1.0, 1.0, 1.0, 1.0);
}

void main() { FragColor = fxaa(screenTexture, TexCoords); }