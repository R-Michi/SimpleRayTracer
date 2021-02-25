#ifndef __ray_tracer_light__
#define __ray_tracer_light__

#undef __STRICT_ANSI__
#include <cmath>
#include <glm/glm.hpp>

struct Light
{
    glm::vec3 direction;
    glm::vec3 intensity;
};

struct Material
{
    glm::vec3 albedo;
    float roughness;
    float metallic;
};

/* Light functions */
float distribution_GGX(const glm::vec3& N, const glm::vec3& H, float r)
{
    const float a = r*r;
    const float a2 = a*a;
    const float NdotH = glm::max(glm::dot(N, H), 0.0f);
    const float NdotH2 = NdotH * NdotH;
    const float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    
    return a2 / (M_PI * denom * denom);
}

float geometry_schlick_GGX(float NdotV, float r)
{
    //const float roughness = (r + 1.0f);
    const float k = (r*r) / 8.0f;
    const float denom = NdotV * (1.0f - k) + k;
    return NdotV / denom;
}

float geometry_smith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float r)
{
    const float NdotV = glm::max(glm::dot(N, V), 0.0f);
    const float NdotL = glm::max(glm::dot(N, L), 0.0f);
    const float ggx1 = geometry_schlick_GGX(NdotV, r);
    const float ggx2 = geometry_schlick_GGX(NdotL, r);
    return ggx1 * ggx2;
}

glm::vec3 frensel_schlick(const glm::vec3& H, const glm::vec3& V, const glm::vec3& F0)
{
    const float cos_theta_inv = 1.0f - glm::max(glm::dot(H, V), 0.0f);
    return F0 + (1.0f - F0) * powf(glm::max(cos_theta_inv, 0.0f), 5.0f);
}

glm::vec3 light(const Light& L, const Material& M, const glm::vec3& V, const glm::vec3 N)
{
    // N & V are normalized

    glm::vec3 F0(0.04f, 0.04f, 0.04f);
    F0 = glm::mix(F0, M.albedo, M.metallic);
    glm::vec3 L0(0.0f, 0.0f, 0.0f);

    glm::vec3 H = glm::normalize(L.direction + V);
    glm::vec3 radiance = L.intensity;

    const float NDF = distribution_GGX(N, H, M.roughness);
    const float G = geometry_smith(N, V, L.direction, M.roughness);
    glm::vec3 F = frensel_schlick(H, V, F0);

    glm::vec3 kd = glm::vec3(1.0f, 1.0f, 1.0f) - F;
    kd *= 1.0f - M.metallic;

    glm::vec3 numerator = NDF * F * G;
    const float denom = 4.0f * glm::max(glm::dot(V, N), 0.0f) * glm::max(glm::dot(L.direction, N), 0.0f);
    glm::vec3 specular = numerator / glm::max(denom, 0.001f);

    float NdotL = glm::max(glm::dot(N, L.direction), 0.0f);
    L0 += (kd * (M.albedo / (float)M_PI) + specular) * radiance * NdotL;
    
    return L0;
}

#endif // __ray_tracer_light__