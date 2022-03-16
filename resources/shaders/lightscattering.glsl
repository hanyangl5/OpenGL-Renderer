
#version 460 core

layout(location = 0) in vec2 TexCoords;
out vec4 FragColor;

struct PointLight {
  vec3 Position;
  vec3 Color;
};

struct DirectLight {
  vec3 Direction;
  vec3 Color;
};
struct ScatteringContext {
  float rayleigh_height;
  float mie_height;
  float sphere_radius;
  float atmosphere_radius;
  vec3 sphere_center;
  int NUM_OUT_SCATTER;
  int NUM_IN_SCATTER;
};

const int NR_LIGHTS = 16;
uniform PointLight point_lights[NR_LIGHTS];
uniform DirectLight direct_lights[NR_LIGHTS];
uniform ScatteringContext scattering_context;
uniform mat4 lightMVP;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughness;
uniform sampler2D shadow_map;

uniform vec3 cam_pos;

const float PI = 3.14159265359;
const float max_dist = 1e6;
const float eps = 1e-6;
vec2 RaySphereIntersect(vec3 oc, vec3 dir, float rad) {

  float b = dot(oc, dir);
  float c = dot(oc, oc) - rad * rad;

  float d = b * b - c;

  if (d < eps) {
    return vec2(max_dist, -max_dist);
  } else {
    d = sqrt(d);
    return vec2(-b - d, -b + d);
  }

  // return vec2(max((-b - sqrt(d)), 0.0), min((-b + sqrt(d)), max_dist));
}

// Mie Sthetaattering Phase Function
float GetMiePhase(float g, float theta) {
  float g2 = g * g;
  float nom = 3.0 * (1.0 - g2) * (1.0 + theta * theta);
  float denom = (1.0 + g2 - 2.0 * g * theta);
  denom += sqrt(denom);
  denom *= 8.0 * PI * (2.0 + g2);
  return nom / denom;
}

// Rayleigh Scattering Phase Function
float GetRayleighPhase(float theta) {
  float nom = 3.0 * (1.0 + theta * theta);
  float denom = 16.0 * PI;
  return nom / denom;
}

// calculate the particle density, and add it
float density(float height, float ph) { return exp(-height / ph); }

float optic(vec3 p, vec3 q, float ph) {
  vec3 s = (q - p) / float(scattering_context.NUM_OUT_SCATTER);
  vec3 v = p + s * 0.5;

  float sum = 0.0;
  for (int i = 0; i < scattering_context.NUM_OUT_SCATTER; i++) {
    float height = max(length(v) - scattering_context.sphere_radius, 0.0);
    sum += density(height, ph);
    v += s;
  }
  sum *= length(s);

  return sum;
}

vec3 in_scatter(vec3 o, vec3 dir, vec2 e, vec3 l) {

  const float ph_ray = 0.05;
  const float ph_mie = 0.02;

  const vec3 rayleigh_scattering_coefficient = vec3(3.8, 13.5, 33.1);
  const vec3 mie_scattering_coefficient = vec3(21.0);
  const float mie_scattering_coefficient_ex = 1.1;

  vec3 rayleigh_attenuation = vec3(0.0);
  vec3 mie_attenuation = vec3(0.0);

  float d_pa_ray = 0.0;
  float d_pa_mie = 0.0;

  float len = (e.y - e.x) /
              float(scattering_context.NUM_IN_SCATTER); // light path length
  vec3 ds = dir * len;                                  // step size along dir
  vec3 v = o + dir * (e.x + len * 0.5);                 // start point

  float height = max(length(v) - scattering_context.sphere_radius, 0.0);

  for (int i = 0; i < scattering_context.NUM_IN_SCATTER; i++) {

    // attenuation of PA
    d_pa_ray += density(height, scattering_context.rayleigh_height) * len;
    d_pa_mie += density(height, scattering_context.mie_height) * len;

    // CP intersect with atmosphere
    vec2 f = RaySphereIntersect(v, l, scattering_context.atmosphere_radius);
    vec3 u = v + l * f.y;

    // attenuation of CP
    float d_cp_ray = optic(v, u, scattering_context.rayleigh_height);
    float d_cp_mie = optic(v, u, scattering_context.mie_height);

    vec3 att = exp(-(d_pa_ray + d_cp_ray) * rayleigh_scattering_coefficient -
                   (d_pa_mie + d_cp_mie) * mie_scattering_coefficient *
                       mie_scattering_coefficient_ex);

    // attenuation of PA
    rayleigh_attenuation += d_pa_ray * att;
    mie_attenuation += d_pa_mie * att;
    v += ds;
  }

  float theta = dot(dir, -l);
  vec3 scatter =
      rayleigh_attenuation * rayleigh_scattering_coefficient *
          GetRayleighPhase(theta) +
      mie_attenuation * mie_scattering_coefficient * GetMiePhase(-0.78, theta);
  // return vec3(1.0, 0.0, 0.0);
  return 1000.0 * scatter;
}

void main() {
  vec3 res = vec3(0.0, 0.0, 0.0);
  // vec3 albedo = texture(gAlbedo, TexCoords).rgb;
  vec3 world_pos = texture(gPosition, TexCoords).rgb;
  if (world_pos == vec3(0.0, 0.0, 0.0)) {
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }
  vec3 view_dir = normalize(cam_pos - world_pos);

  vec3 oc = cam_pos - scattering_context.sphere_center;

  // intersect with atmosphere
  vec2 outer =
      RaySphereIntersect(oc, view_dir, scattering_context.atmosphere_radius);
  if (outer.x > outer.y) {
    FragColor = vec4(0.0, 0.0, 0.0, 0.0);
    return;
  }
  vec2 inner =
      RaySphereIntersect(oc, view_dir, scattering_context.sphere_radius);

  // start point and end point
  vec2 intersection = vec2(outer.x, min(outer.y, inner.x));
  // radiance at shading point P
  vec3 I =
      in_scatter(cam_pos, view_dir, intersection, direct_lights[0].Direction);
  // I = vec3(1.0, 1.0, 1.0);
  res += I;
  FragColor = vec4(res, 1.0);
  return;
}
