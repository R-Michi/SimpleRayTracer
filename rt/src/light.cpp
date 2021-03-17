#include "../light.h"

float rt::__rt_internal::distribution_GGX(const glm::vec3& N, const glm::vec3& H, float r) noexcept
{
    // r ... roughness
    //NDF_GGXTR(N,H,α) = α² / (π*((N*H)²*(α²−1)+1)²)
    // α = r² ... squaring the roughness value gives more realistic results

    const float a       = r*r;
    const float a2      = a*a;
    const float NdotH   = glm::max(glm::dot(N, H), 0.0f);
    const float NdotH2  = NdotH * NdotH;
    const float denom   = (NdotH2 * (a2 - 1.0f) + 1.0f);

    return a2 / (M_PI * denom * denom);
}

float rt::__rt_internal::geometry_schlick_GGX(float NdotV, float r) noexcept
{
    // r ... roughness
    // G_SchlickGGX(n,v,k) = (N*V) / ((N*V)*(1−k)+k)
    // k = (r+1)² / 8

    const float roughness   = (r + 1.0f);
    const float k           = (roughness*roughness) / 8.0f;
    const float denom       = NdotV * (1.0f - k) + k;
    return NdotV / denom;
}

float rt::__rt_internal::geometry_smith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float r) noexcept
{
    // G = G_sub(N,V,r) * G_sub(N,L,r)

    const float NdotV   = glm::max(glm::dot(N, V), 0.0f);
    const float NdotL   = glm::max(glm::dot(N, L), 0.0f);
    const float ggx1    = geometry_schlick_GGX(NdotV, r);
    const float ggx2    = geometry_schlick_GGX(NdotL, r);
    return ggx1 * ggx2;
}

glm::vec3 rt::__rt_internal::frensel_schlick(const glm::vec3& H, const glm::vec3& V, const glm::vec3& F0) noexcept
{
    // F_Schlick(H,V,F0) = F0+(1−F0)*(1−(H*V))^2

    const float HdotV = 1.0f - glm::max(glm::dot(H, V), 0.0f);
    return F0 + (1.0f - F0) * powf(glm::max(HdotV, 0.0f), 5.0f);
}

glm::vec3 rt::light(const Light& L, const Material& M, const glm::vec3& V, const glm::vec3 N) noexcept
{
    static glm::vec3 _F0(0.04f);
    static glm::vec3 F0 = glm::mix(_F0, M.albedo, M.metallic);

    glm::vec3 H = glm::normalize(L.direction + V);
    glm::vec3 radiance = L.intensity;

    const float NDF = __rt_internal::distribution_GGX(N, H, M.roughness);
    const float G   = __rt_internal::geometry_smith(N, V, L.direction, M.roughness);
    glm::vec3 F     = __rt_internal::frensel_schlick(H, V, F0);

    glm::vec3 kd = glm::vec3(1.0f) - F;
    kd *= 1.0f - M.metallic;

    glm::vec3 numerator = NDF * F * G;
    const float denom   = 4.0f * glm::max(glm::dot(V, N), 0.0f) * glm::max(glm::dot(L.direction, N), 0.0f);
    glm::vec3 specular  = numerator / glm::max(denom, 0.001f);

    float NdotL = glm::max(glm::dot(N, L.direction), 0.0f);
    return (kd * (M.albedo / (float)M_PI) + specular) * radiance * NdotL;
}