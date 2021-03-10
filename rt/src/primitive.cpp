#include "../primitive.h"

#ifdef __STRICT_ANSI__
    #undef __STRICT_ANSI__
#endif

#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

using namespace rt;

/* SPHERE */

Sphere::Sphere(void) noexcept : Sphere({0.0f, 0.0f, 0.0f}, 0.0f)
{
    // no further code
}

Sphere::Sphere(const glm::vec3& center, float radius) noexcept
{
    this->set(center, radius);
    this->_mtl = {{0.0f, 0.0f, 0.0f}, 1.0f, 0.0f, 1.0f};
}

Sphere::Sphere(const glm::vec3& center, float radius, const Material& mtl) noexcept
{
    this->set(center, radius, mtl);
}

Sphere::Sphere(const Sphere& sphere) noexcept
{
    *this = sphere;
}   

Sphere& Sphere::operator= (const Sphere& sphere)  noexcept
{
    this->_center   = sphere._center;
    this->_radius   = sphere._radius;
    this->_mtl      = sphere._mtl;
    return *this;
}

Sphere::Sphere(Sphere&& sphere) noexcept
{
    *this = sphere;
}

Sphere& Sphere::operator= (Sphere&& sphere) noexcept
{
    this->_center   = sphere._center;
    sphere._center  = {0.0f, 0.0f, 0.0f};

    this->_radius   = sphere._radius;
    sphere._radius  = 0.0f;

    this->_mtl      = sphere._mtl;
    sphere._mtl     = {{0.0f, 0.0f, 0.0f}, 1.0f, 0.0f, 1.0f};
    return *this;
}

Sphere::~Sphere(void) noexcept
{
    // dtor not used
}

void Sphere::set(const glm::vec3& center, float radius) noexcept
{
    this->_center = center;
    this->_radius = radius;
}

void Sphere::set(const glm::vec3& center, float radius, const Material& mtl) noexcept
{
    this->_center   = center;
    this->_radius   = radius;
    this->_mtl      = mtl;
}

void Sphere::set_center(const glm::vec3& center) noexcept
{
    this->_center = center;
}

void Sphere::set_radius(float radius) noexcept
{
    this->_radius = radius;
}

void Sphere::set_material(const Material& mtl) noexcept
{
    this->_mtl = mtl;
}

float Sphere::_intersect(const Ray& ray, float t_max, int flags) const
{
    /*  The intersecion point is a quadratic function: ax^2 + bx + c = 0
        These following lines calculate the parameter a, b, c of that function 
        The parameter 'a' is not calculated because ray.direction * ray.direction is always be 1 because the direction
        of the ray is a unit vector. */
    glm::vec3 oc        = ray.origin - this->_center;  
    const float b       = 2 * glm::dot(ray.direction, oc);
    const float c       = glm::dot(oc, oc) - this->_radius * this->_radius;
    const float delta   = b*b - 4*c;

    // if ray intersects with the sphere
    if(delta >= 0.0f)
    {
        // calculate both results of the quadratic equation
        float t0, t1;                                   // both resuls of the quadratic function
        if(delta == 0.0f)                               // just an optimization if both solutions are the same
            t0 = t1 = -0.5f * b;
        else                                            // if they are not the same, use the standard formula
        {
            t0 = (-b - sqrt(delta)) / 2;
            t1 = (-b + sqrt(delta)) / 2;
        }

        /** 
         *  The intersection test can be made with fewer lines of code.
         *  However, my aim was to minimize the number of comparisons needed to get the final result.
         * 
         *  The first check is if the pixel is within the render distance, if not there are no more comparions needed.
         *  The second check is if the sphere in front of us.
         *  The third check is if we are within the sphere.
         * 
         *  Note: The order of the second and third check is important, since there are more objects in front of us, than within us.
         *  This will result in less comparisons in total.
         * 
         *  Also important is that we are testing the closest intersection t0 at first as this determines if we are in front of or
         *  within the sphere. This also results in less comparisons.
         */
        if(t0 < t_max)
        {
            if(t0 >= 0.0f && t1 >= 0.0f)                                                // in front of the sphere
                return t0;
            else if(flags & RT_INTERSECTION_CONSIDER_INSIDE && t0 < 0.0f && t1 >= 0.0f) // within the sphere
                return t1;
        }
    }
    return t_max;
}

float Sphere::intersect(const Ray& ray, float t_max, int flags) const
{
    return this->_intersect(ray, t_max, flags);
}

float Sphere::distance(const glm::vec3& p) const
{
    return glm::length(this->_center - p) - this->_radius;
}

/* DISTANCED SPHERE */

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

/* INFINITE PLANE */

InfPlane::InfPlane(void) noexcept : InfPlane({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f})
{
    // no further code
}

InfPlane::InfPlane(const glm::vec3& direction, const glm::vec3& origin) noexcept
{
    this->set(direction, origin);
    this->_mtl          = {{0.0f, 0.0f, 0.0f}, 1.0f, 0.0f, 1.0f};
}

InfPlane::InfPlane(const glm::vec3& direction, const glm::vec3& origin, const Material& mtl) noexcept
{
    this->set(direction, origin, mtl);
}

InfPlane::InfPlane(const InfPlane& inf_plane) noexcept
{
    *this = inf_plane;
}

InfPlane& InfPlane::operator= (const InfPlane& inf_plane) noexcept
{
    this->_direction    = inf_plane._direction;
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

    this->_mtl              = inf_plane._mtl;
    this->_mtl              = {{0.0f, 0.0f, 0.0f}, 1.0f, 0.0f, 1.0f};
    return *this;
}

InfPlane::~InfPlane(void) noexcept
{
    // dotr not needed
}

void InfPlane::set(const glm::vec3& direction, const glm::vec3& origin) noexcept
{
    this->_direction = direction;
}

void InfPlane::set(const glm::vec3& direction, const glm::vec3& origin, const Material& mtl) noexcept
{
    this->_direction = direction;
    this->_mtl = mtl;
}

void InfPlane::set_direction(const glm::vec3& direction) noexcept
{
    this->_direction = direction;
}

void InfPlane::set_material(const Material& mtl) noexcept
{
    this->_mtl = mtl;
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
