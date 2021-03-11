#include "../primitive.h"

#include <cmath>

using namespace rt;

InfPlane::InfPlane(void) noexcept : Primitive()
{
    this->set({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f});
}

InfPlane::InfPlane(const glm::vec3& direction, const glm::vec3& origin) noexcept : Primitive()
{
    this->set(direction, origin);
}

InfPlane::InfPlane(const glm::vec3& direction, const glm::vec3& origin, const Material& mtl) noexcept : Primitive(mtl)
{
    this->set(direction, origin);
}

InfPlane::InfPlane(const InfPlane& inf_plane) noexcept
{
    *this = inf_plane;
}

InfPlane& InfPlane::operator= (const InfPlane& inf_plane) noexcept
{
    this->_direction    = inf_plane._direction;
    this->_origin       = inf_plane._origin;
    this->_mtl          = inf_plane._mtl;
    return *this;
}

InfPlane::InfPlane(InfPlane&& inf_plane) noexcept
{
    *this = inf_plane;
}

InfPlane& InfPlane::operator= (InfPlane&& inf_plane) noexcept
{
    this->_direction        = inf_plane._direction;
    inf_plane._direction    = {0.0f, 0.0f, 0.0f};

    this->_origin           = inf_plane._origin;
    inf_plane._origin       = {0.0f, 0.0f, 0.0f};

    this->_mtl              = inf_plane._mtl;
    this->_mtl              = {{0.0f, 0.0f, 0.0f}, 1.0f, 0.0f, 1.0f};
    return *this;
}

void InfPlane::set(const glm::vec3& direction, const glm::vec3& origin) noexcept
{
    this->_direction    = direction;
    this->_origin       = origin;
}

void InfPlane::set(const glm::vec3& direction, const glm::vec3& origin, const Material& mtl) noexcept
{
    this->_direction    = direction;
    this->_origin       = origin;
    this->_mtl          = mtl;
}

void InfPlane::set_direction(const glm::vec3& direction) noexcept
{
    this->_direction = direction;
}

void InfPlane::set_origin(const glm::vec3& origin) noexcept
{
    this->_origin = origin;
}

float InfPlane::intersect(const Ray& ray, float t_max, int flags) const
{
    const float denom = glm::dot(this->_direction, ray.direction);
    // If the denominator is smaller or equal 0.0 then the intersection point is infinitely far away.
    // That basically means the ray is parallel to the plane and there is no intersection.
    return (denom > 0.0f) ? -glm::dot(this->_direction, this->_origin - ray.origin) / denom : t_max;
}

float InfPlane::distance(const glm::vec3& p) const
{
    return abs(glm::dot(this->_direction, p - this->_origin));
}