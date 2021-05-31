#version 460 core

layout(location = 0) in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;
uniform sampler2D shadow_map;

uniform mat4 lightMVP;

struct PointLight {
  vec3 Position;
  vec3 Color;
};
struct DirectLight {
  vec3 Direction;
  vec3 Color;
};

const int NR_LIGHTS = 16;
uniform PointLight point_lights[NR_LIGHTS];
uniform DirectLight direct_lights[NR_LIGHTS];
uniform vec3 viewPos;

const float PI = 3.14159265359;

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
    poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
    radius += radiusStep;
    angle += ANGLE_STEP;
  }
}

float ShadowMap(sampler2D shadowMap, vec3 shadowCoord) {
  // deeper than shadowmap
  if (shadowCoord.z - texture2D(shadowMap, shadowCoord.xy).r > 0.005) {
    return 0.0;
  }
  return 1.0;
}

float PCF(sampler2D shadowMap, vec3 coords, float filter_size) {
  int coverage_count = 0;
  poissonDiskSamples(coords.xy);
  for (int i = 0; i < sample_point; i++) {
    if (coords.z -
            texture2D(shadowMap, coords.xy + poissonDisk[i] * filter_size).r >
        0.01) {
      coverage_count++;
    }
  }
  return 1.0 - float(coverage_count) / float(sample_point);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float num = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float num = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

void main() {

  // vec3 albedo = pow(texture(gAlbedo, TexCoords).rgb, vec3(2.2));
  vec3 albedo = texture(gAlbedo, TexCoords).rgb;
  float metallic = texture(gMetallicRoughness, TexCoords).r;
  float roughness = texture(gMetallicRoughness, TexCoords).g;
  vec3 FragPos = texture(gPosition, TexCoords).rgb;
  vec3 Normal = texture(gNormal, TexCoords).rgb;

  vec3 N = normalize(Normal);
  vec3 V = normalize(viewPos - FragPos);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // reflectance equation
  vec3 Lo = vec3(0.0);

  // point light
  for (int i = 0; i < NR_LIGHTS; ++i) {
    // calculate per-light radiance
    vec3 L = normalize(point_lights[i].Position - FragPos);
    vec3 H = normalize(V + L);
    float distance = length(point_lights[i].Position - FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = point_lights[i].Color * attenuation;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  }

  // dirctlight
  for (int i = 0; i < NR_LIGHTS; ++i) {
    vec3 L = -normalize(direct_lights[i].Direction);
    vec3 H = normalize(V + L);
    vec3 radiance = direct_lights[i].Color;

    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);

    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
  }
  vec3 color = Lo;

  vec4 light_space_pos = lightMVP * vec4(FragPos, 1.0);
  vec3 shadowcoord = (light_space_pos * 0.5 + 0.5).xyz;
  // float visibility = ShadowMap(shadow_map, shadowcoord);
  float visibility = PCF(shadow_map, shadowcoord, 0.005);
  FragColor = max(0.5, visibility) * vec4(color, 1.0);
  // FragColor = visibility * vec4(color, 1.0);
}
