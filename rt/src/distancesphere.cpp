#include "../primitive.h"

using namespace rt;

DistanceSphere::DistanceSphere(void) noexcept
: Sphere() {}

DistanceSphere::DistanceSphere(const glm::vec3& center, float radius) noexcept
: Sphere(center, radius) {}

DistanceSphere::DistanceSphere(const glm::vec3& center, float radius, const Material& mtl) noexcept
: Sphere(center, radius, mtl) {}

DistanceSphere::DistanceSphere(const DistanceSphere& sphere) noexcept
: Sphere(sphere) {}

DistanceSphere::DistanceSphere(DistanceSphere&& sphere) noexcept
: Sphere(sphere) {}

float DistanceSphere::intersect(const Ray& ray, float t_max, int flags) const
{
    const float d = glm::length(this->_center - ray.origin) - this->_radius;
    // early test if the whole sphere is out of the render distance
    return (d < t_max) ? _intersect(ray, t_max, flags) : t_max;
}