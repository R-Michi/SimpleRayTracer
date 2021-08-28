/**
* @file     sphere.cpp
* @brief    Implementation of the sphere primitive.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#include "sphere.h"
#include <iostream>

using namespace rt;

Sphere::Sphere(void) noexcept : Primitive()
{
    this->set({0.0f, 0.0f, 0.0f}, 0.0f);
}

Sphere::Sphere(const glm::vec3& center, float radius) noexcept : Primitive()
{
    this->set(center, radius);
}

Sphere::Sphere(const glm::vec3& center, float radius, const PrimitiveAttribute& attrib) noexcept : Primitive(attrib)
{
    this->set(center, radius);
}

Sphere::Sphere(const Sphere& sphere) noexcept
{
    *this = sphere;
}   

Sphere& Sphere::operator= (const Sphere& sphere)  noexcept
{
    this->_center   = sphere._center;
    this->_radius   = sphere._radius;
    this->set_attribute(sphere.attribute());
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

    this->set_attribute(sphere.attribute());
    return *this;
}

void Sphere::set(const glm::vec3& center, float radius) noexcept
{
    this->_center = center;
    this->_radius = radius;
}

void Sphere::set(const glm::vec3& center, float radius, const PrimitiveAttribute& attrib) noexcept
{
    this->_center   = center;
    this->_radius   = radius;
    this->set_attribute(attrib);
}

void Sphere::set_center(const glm::vec3& center) noexcept
{
    this->_center = center;
}

void Sphere::set_radius(float radius) noexcept
{
    this->_radius = radius;
}

float Sphere::_intersect(const ray_t& ray, float t_max, RayCullMask cull_mask, RayHitInformation& hit_info) const
{
    /*  The intersecion point is a quadratic function: ax^2 + bx + c = 0
        These following lines calculate the parameter a, b, c of that function 
        The parameter 'a' is not calculated because ray.direction * ray.direction is always 1 because the direction
        of the ray is a unit vector. */
    glm::vec3 oc        = ray.origin - this->_center;  
    const float b       = 2 * glm::dot(ray.direction, oc);
    const float c       = glm::dot(oc, oc) - this->_radius * this->_radius;
    const float delta   = b*b - 4*c;
    hit_info = 0x0;

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
            if (!(cull_mask & RT_CULL_MASK_FRONT_BIT) && t0 >= 0.0f && t1 >= 0.0f)       // in front of the sphere
            {
                hit_info |= RT_HIT_INFO_FRONT_BIT;
                return t0;
            }
            else if (!(cull_mask & RT_CULL_MASK_BACK_BIT) && t0 < 0.0f && t1 >= 0.0f)   // within the sphere
            {
                hit_info |= RT_HIT_INFO_BACK_BIT;
                return t1;
            }
        }
        hit_info = RT_HIT_INFO_NONE;
    }
    return t_max;
}

float Sphere::intersect(const ray_t& ray, float t_max, RayCullMask cull_mask, RayHitInformation& hit_info) const
{
    return this->_intersect(ray, t_max, cull_mask, hit_info);
}

float Sphere::distance(const glm::vec3& p) const
{
    return glm::length(this->_center - p) - this->_radius;
}