/**
* @file     infplane.h
* @brief    This file contains the infinite plane primitive.
*           The infinite plane primitive represents a plane that expands
*           infinitily wide in every direction.
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
    class InfPlane : public Primitive
    {
    private:
        glm::vec3 _direction;   // The direction of the plane.
        glm::vec3 _origin;      // The plane's origin
    
    public:
        InfPlane(void) noexcept;

        /**
         *  @param[in] direction: The direction in which the plane points.
         *  @param[in] origin: The plane's origin in 3D-space.
         */
        InfPlane(const glm::vec3& direction, const glm::vec3& origin) noexcept;

        /**
         *  @param[in] direction: The direction in which the plane points.
         *  @param[in] origin: The plane's origin in 3D-space.
         *  @param[in] attrib: primitive attribute pointer
         */
        InfPlane(const glm::vec3& direction, const glm::vec3& origin, const PrimitiveAttribute* attrib) noexcept;
        
        InfPlane(const InfPlane& inf_plane) noexcept;
        InfPlane& operator= (const InfPlane& inf_plane) noexcept;
        InfPlane(InfPlane&& inf_plane) noexcept;
        InfPlane& operator= (InfPlane&& inf_plane) noexcept;

        virtual ~InfPlane(void) noexcept {}

        /**
         *  @brief Sets the properties of the plane.
         *  @param[in] direction: The direction in which the plane points.
         *  @param[in] origin: The plane's origin in 3D-space.
         */
        void set(const glm::vec3& direction, const glm::vec3& origin) noexcept;

        /**
         *  @param[in] direction: The direction in which the plane points.
         *  @param[in] origin: The plane's origin in 3D-space.
         *  @param[in] attrib: primitive attribute pointer
         */
        void set(const glm::vec3& direction, const glm::vec3& origin, const PrimitiveAttribute* attrib) noexcept;

        /**
         *  @brief Sets the direction of the plane.
         *  @param[in] direction: The direction in which the plane points.
         */
        void set_direction(const glm::vec3& direction) noexcept;

        /**
         *  @brief Sets the plane's origin in 3D-space.
         *  @param[in] origin: The plane's origin in 3D-space.
         */
        void set_origin(const glm::vec3& origin) noexcept;

        /** @return The plane's direction. */
        inline const glm::vec3& direction(void) const noexcept
        {return this->_direction;}

        /** @return The plane's origin in 3D-space. */
        inline const glm::vec3& origin(void) const noexcept
        {return this->_origin;}
        
        /** @brief Intersection test for line - infinite plane - intersection. */
        virtual float intersect(const ray_t& ray, float t_max, int flags) const;

        /** @brief Distance from a 3D-point P to the closest point on the plane. */
        virtual float distance(const glm::vec3& p) const;

        /** @return A dynamic clone of the infinite plane. */
        virtual Primitive* clone_dynamic(void)
        {return new InfPlane(*this);}

        /** @return Size of the current infinite plane. */
        virtual size_t get_sizeof(void)
        {return sizeof(InfPlane);}
    };
}