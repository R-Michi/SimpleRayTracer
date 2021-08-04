/**
* @file     distancesphere.cpp
* @brief    Implementation of the distance-sphere primitive
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#include "distancesphere.h"

using namespace rt;

DistanceSphere::DistanceSphere(void) noexcept
: Sphere() {}

DistanceSphere::DistanceSphere(const glm::vec3& center, float radius) noexcept
: Sphere(center, radius) {}

DistanceSphere::DistanceSphere(const glm::vec3& center, float radius, const PrimitiveAttribute* attrib) noexcept
: Sphere(center, radius, attrib) {}

DistanceSphere::DistanceSphere(const DistanceSphere& sphere) noexcept
: Sphere(sphere) {}

DistanceSphere::DistanceSphere(DistanceSphere&& sphere) noexcept
: Sphere(sphere) {}

float DistanceSphere::intersect(const ray_t& ray, float t_max, int flags) const
{
    const float d = glm::length(this->_center - ray.origin) - this->_radius;
    // early test if the whole sphere is out of the render distance
    return (d < t_max) ? _intersect(ray, t_max, flags) : t_max;
}