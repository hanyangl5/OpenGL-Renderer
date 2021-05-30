#version 460 core

layout(location = 0) in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;

struct PointLight {
  vec3 Position;
  vec3 Color;
};
struct DirectLight {
  vec3 Direction;
  vec3 Color;
};

const int NR_LIGHTS = 32;
uniform PointLight point_lights[NR_LIGHTS];
uniform DirectLight direct_lights[NR_LIGHTS];
uniform vec3 viewPos;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

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
  FragColor = vec4(color, 1.0);
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