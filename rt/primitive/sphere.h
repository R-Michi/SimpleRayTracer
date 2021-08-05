/**
* @file     sphere.h
* @brief    This file contains the sphere primitive.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "primitive.h"

namespace rt
{
    class Sphere : public Primitive
    {
    protected:
        glm::vec3 _center;  // Center of the sphere
        float _radius;      // Radius of the sphere

        // Intersection test for ray - sphere - intersection.
        float _intersect(const ray_t& ray, float t_max, int flags) const;

    public:
        Sphere(void) noexcept;

        /**
         *  @param[in] center: Center of the sphere in the 3D-space.
         *  @param[in] radius: Radius of the sphere.
         */
        Sphere(const glm::vec3& center, float radius) noexcept;

        /**
         *  @param[in] center: Center of the sphere in the 3D-space.
         *  @param[in] radius: Radius of the sphere.
         *  @param[in] attrib: primitive attribute
         */
        Sphere(const glm::vec3& center, float radius, const PrimitiveAttribute& attrib) noexcept;

        Sphere(const Sphere& sphere) noexcept;
        Sphere& operator= (const Sphere& sphere) noexcept;
        Sphere(Sphere&& sphere) noexcept;
        Sphere& operator= (Sphere&& sphere) noexcept;

        virtual ~Sphere(void) noexcept {}

        /**
         *  @brief Sets the properties of the sphere.
         *  @param[in] center: Center of the sphere in the 3D-space.
         *  @param[in] radius: Radius of the sphere.
         */
        void set(const glm::vec3& center, float radius) noexcept;

        /**
         *  @param[in] center: Center of the sphere in the 3D-space.
         *  @param[in] radius: Radius of the sphere.
         *  @param[in] attrib: primitive attribute
         */
        void set(const glm::vec3& center, float radius, const PrimitiveAttribute& attrib) noexcept;

        /**
         *  @brief Sets the center of the sphere.
         *  @param[in] center : Center of the sphere in the 3D-space.
         */ 
        void set_center(const glm::vec3& center) noexcept;

        /**
         *  @brief Sets the radius of the sphere.
         *  @param radius Radius of the sphere.
         */
        void set_radius(float radius) noexcept;
        
        /** @return The center of the sphere in 3D-Space. */
        inline const glm::vec3& center(void) const noexcept
        {return this->_center;}

        /** @return -> The radius of the sphere. */
        inline float radius(void) const noexcept
        {return this->_radius;}

        /** @brief Intersection test for ray - sphere - intersection. */
        virtual float intersect(const ray_t& ray, float t_max, int flags) const;

        /** @brief Distance from a 3D-point P to the closest point on the sphere. */
        virtual float distance(const glm::vec3& p) const;
        
        /** @return A dynamic clone of the sphere. */
        virtual Primitive* clone_dynamic(void)
        {return new Sphere(*this);}

        /** @return -> Size of the current sphere. */
        virtual size_t get_sizeof(void)
        {return sizeof(Sphere);}
    };
}