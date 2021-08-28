/**
* @file     distancesphere.h
* @brief    This file contains a sphere primitive with an early test if the
*           sphere's center is out of the render distance.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#pragma once

#include "sphere.h"

namespace rt
{
    class DistanceSphere : public Sphere
    {
    public:
        DistanceSphere(void) noexcept;

        /**
         *  @param[in] center: Center of the sphere in the 3D-space.
         *  @param[in] radius: Radius of the sphere.
         */
        DistanceSphere(const glm::vec3& center, float radius) noexcept;

        /**
         *  @param[in] center: Center of the sphere in the 3D-space.
         *  @param[in] radius: Radius of the sphere.
         *  @param[in] attrib: primitive attribute
         */
        DistanceSphere(const glm::vec3& center, float radius, const PrimitiveAttribute& attrib) noexcept;

        DistanceSphere(const DistanceSphere& sphere) noexcept;
        DistanceSphere(DistanceSphere&& sphere) noexcept;
        virtual ~DistanceSphere(void) noexcept {}

        /** @brief Intersection test for ray - sphere - intersection. */
        virtual float intersect(const ray_t& ray, float t_max, RayCullMask cull_mask, RayHitInformation& hit_info) const;

        /** @return A dynamic clone of the distance sphere. */
        virtual Primitive* clone_dynamic(void)
        {return new DistanceSphere(*this);}

        /** @return Size of the current distance sphere. */
        virtual size_t get_sizeof(void)
        {return sizeof(DistanceSphere);}
    };
}