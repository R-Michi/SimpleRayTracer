#ifndef __rt_primitive_h__
#define __rt_primitive_h__

#include "rt_types.h"

namespace rt
{
    class Primitive
    {
    protected:
        Material _mtl;

    public:
        /**
         *  Default constructor.
         *  No parameter.
         */
        Primitive(void) noexcept;

        /**
         *  @param mtl -> The primitives material properties. 
         */
        Primitive(const Material& mtl) noexcept;

        // Destructor not used.
        virtual ~Primitive(void) noexcept {}

        /**
         *  Sets the primitive's material properties.
         *  @param mtl -> The sphere's material properties.
         */
        void set_material(const Material& mtl) noexcept;

        /// @return -> The primitive's material prperties
        inline const Material& material(void) const noexcept
        {return this->_mtl;}

        /**
         *  Executes the intersection test for the current primitive.
         *  @param ray -> The ray that is tested if it intersects with a primitive.
         *  @param t_max -> The maximum length of the ray. Similar to the render distance.
         *  @param flags -> Can change the way how intersection is calculated.
         *  @return -> The length from the ray-origin to the closest intersection point.
         *             The length caps at @param t_max if the ray does not intersect with any geometry.
         */
        virtual float intersect(const Ray& ray, float t_max, int flags) const = 0;

        /**
         *  Calculates the distance from a 3D-point P to the current primitive.
         *  @param p -> Point from where the distance should be calculated.
         *  @return -> The length from the point P to the current prmitive.
         *  NOTE: The distance does not cap at t_max.
         */
        virtual float distance(const glm::vec3& p) const = 0;
    };
}

#endif // __rt_primitive_h__