#ifndef __rt_sphere_h__
#define __rt_sphere_h__

#include "primitive.h"

namespace rt
{
    class Sphere : public Primitive
    {
    protected:
        glm::vec3 _center;  // Center of the sphere
        float _radius;      // Radius of the sphere

        // Intersection test for line(ray) - sphere - intersection.
        float _intersect(const Ray& ray, float t_max, int flags) const;

    public:
        /**
         *  Default constructor.
         *  No parameter.
         */
        Sphere(void) noexcept;

        /**
         *  @param center -> Center of the sphere in the 3D-space.
         *  @param radius -> Radius of the sphere.
         */
        Sphere(const glm::vec3& center, float radius) noexcept;

        /**
         *  @param center -> Center of the sphere in the 3D-space.
         *  @param radius -> Radius of the sphere.
         *  @param mtl -> The sphere's material properties.
         */
        Sphere(const glm::vec3& center, float radius, const Material& mtl) noexcept;

        // Copy constrcutor and copy operator.
        Sphere(const Sphere& sphere) noexcept;
        Sphere& operator= (const Sphere& sphere) noexcept;

        // Move constructor and move operator.
        Sphere(Sphere&& sphere) noexcept;
        Sphere& operator= (Sphere&& sphere) noexcept;

        // Destructor, not used.
        virtual ~Sphere(void) noexcept {}

        /**
         *  Sets the properties of the sphere.
         *  @param center -> Center of the sphere in the 3D-space.
         *  @param radius -> Radius of the sphere.
         */
        void set(const glm::vec3& center, float radius) noexcept;

        /**
         *  @param center -> Center of the sphere in the 3D-space.
         *  @param radius -> Radius of the sphere.
         *  @param mtl -> The sphere's material properties.
         */
        void set(const glm::vec3& center, float radius, const Material& mtl) noexcept;

        /**
         *  Sets the center of the sphere.
         *  @param center -> Center of the sphere in the 3D-space.
         */ 
        void set_center(const glm::vec3& center) noexcept;

        /**
         *  Sets the radius of the sphere.
         *  @param radius -> Radius of the sphere.
         */
        void set_radius(float radius) noexcept;
        
        /// @return -> The center of the sphere in 3D-Space.
        inline const glm::vec3& center(void) const noexcept
        {return this->_center;}

        /// @return -> The radius of the sphere.
        inline float radius(void) const noexcept
        {return this->_radius;}

        // Intersection test for line(ray) - sphere - intersection.
        virtual float intersect(const Ray& ray, float t_max, int flags) const;

        // Distance from a 3D-point P to the closest point on the sphere.
        virtual float distance(const glm::vec3& p) const;
    };
}

#endif //__rt_sphere_h__