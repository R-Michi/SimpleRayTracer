/**
* @file     primitive.h
* @brief    Base class of every primitive. 
            Every custom made primitive must inherit from the rt::Primitive class.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../misc/rt_types.h"

namespace rt
{
    class Primitive
    {
    private:
        PrimitiveAttribute* attrib;

    protected:
        /**
         *  @brief Sets the primitive's material properties.
         *  @param[in] attrib: primitive attribute pointer
         */
        void set_attribute(const PrimitiveAttribute* attrib) noexcept;

    public:
        Primitive(void) noexcept;

        /** @param[in] attrib: primitive attribute */
        Primitive(const PrimitiveAttribute& attrib) noexcept;
        virtual ~Primitive(void);

        /**
         *  @brief Sets the primitive's material properties.
         *  @param[in] attrib: primitive attribute
         */
        void set_attribute(const PrimitiveAttribute& attrib) noexcept;

        /** @return The primitive's material prperties */
        inline const PrimitiveAttribute* attribute(void) const noexcept
        {return this->attrib;}

        /**
         *  @brief Executes the intersection test for the current primitive.
         *  @param[in] ray: The ray that is tested if it intersects with a primitive.
         *  @param[in] t_max: The maximum length of the ray. Similar to the render distance.
         *  @param[in] flags: Can change the way how intersection is calculated.
         *  @return The length from the ray-origin to the closest intersection point.
         *  The length caps at @param t_max if the ray does not intersect with any geometry.
         */
        virtual float intersect(const ray_t& ray, float t_max, int flags) const = 0;

        /**
         *  @brief Calculates the distance from a 3D-point P to the current primitive.
         *  @param[in] p: Point from where the distance should be calculated.
         *  @return The length from the point P to the current prmitive.
         *  NOTE: The distance does not cap at t_max.
         */
        virtual float distance(const glm::vec3& p) const = 0;

        /** 
         *  @return A dynamic clone of the own instance.
         *  The memory does not free automantically.
         *  To avoid memory leaks YOU have to free it yourself.
         *  HINT: The primitive buffer deletes the memory automatically.
         */
        virtual Primitive* clone_dynamic(void) = 0;

        /** @return The size of the primitive. */
        virtual size_t get_sizeof(void) = 0;
    };
}