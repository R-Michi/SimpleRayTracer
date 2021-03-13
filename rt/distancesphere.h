#ifndef __rt_distance_sphere_h__
#define __rt_distance_sphere_h__

#include "sphere.h"

namespace rt
{
    class DistanceSphere : public Sphere
    {
    public:
        /**
         *  Default constructor.
         *  No parameter.
         */
        DistanceSphere(void) noexcept;

        /**
         *  @param center -> Center of the sphere in the 3D-space.
         *  @param radius -> Radius of the sphere.
         */
        DistanceSphere(const glm::vec3& center, float radius) noexcept;

        /**
         *  @param center -> Center of the sphere in the 3D-space.
         *  @param radius -> Radius of the sphere.
         *  @param mtl -> The sphere's material properties.
         */
        DistanceSphere(const glm::vec3& center, float radius, const Material& mtl) noexcept;

        // Copy constructor.
        DistanceSphere(const DistanceSphere& sphere) noexcept;

        // Move constructor.
        DistanceSphere(DistanceSphere&& sphere) noexcept;

        // destructor not used
        virtual ~DistanceSphere(void) noexcept {}

        // Intersection test for line(ray) - sphere - intersection.
        virtual float intersect(const Ray& ray, float t_max, int flags) const;
    };
}

#endif //__rt_distance_sphere_h__