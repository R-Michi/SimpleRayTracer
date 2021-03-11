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

    class InfPlane : public Primitive
    {
    private:
        glm::vec3 _direction;   // The direction of the plane.
        glm::vec3 _origin;      // The plane's origin
    
    public:
        /**
         *  Default constructor.
         *  No parameter.
         */
        InfPlane(void) noexcept;

        /**
         *  @param direction -> The direction in which the plane points.
         *  @param origin -> The plane's origin in 3D-space.
         */
        InfPlane(const glm::vec3& direction, const glm::vec3& origin) noexcept;

        /**
         *  @param direction -> The direction in which the plane points.
         *  @param origin -> The plane's origin in 3D-space.
         *  @param mtl -> The plane's material properties.
         */
        InfPlane(const glm::vec3& direction, const glm::vec3& origin, const Material& mtl) noexcept;
        
        // Copy constructor and operator.
        InfPlane(const InfPlane& inf_plane) noexcept;
        InfPlane& operator= (const InfPlane& inf_plane) noexcept;

        // Move constructor and operator.
        InfPlane(InfPlane&& inf_plane) noexcept;
        InfPlane& operator= (InfPlane&& inf_plane) noexcept;

        // Destructor, not used.
        virtual ~InfPlane(void) noexcept {}

        /**
         *  Sets the properties of the plane.
         *  @param direction -> The direction in which the plane points.
         *  @param origin -> The plane's origin in 3D-space.
         */
        void set(const glm::vec3& direction, const glm::vec3& origin) noexcept;

        /**
         *  @param direction -> The direction in which the plane points.
         *  @param origin -> The plane's origin in 3D-space.
         *  @param mtl -> The plane's material properties.
         */
        void set(const glm::vec3& direction, const glm::vec3& origin, const Material& mtl) noexcept;

        /**
         *  Sets the direction of the plane.
         *  @param direction -> The direction in which the plane points.
         */
        void set_direction(const glm::vec3& direction) noexcept;

        /**
         *  Sets the plane's origin in 3D-space.
         *  @param origin -> The plane's origin in 3D-space.
         */
        void set_origin(const glm::vec3& origin) noexcept;

        /// @return -> The plane's direction.
        inline const glm::vec3& direction(void) const noexcept
        {return this->_direction;}

        /// @return -> The plane's origin in 3D-space.
        inline const glm::vec3& origin(void) const noexcept
        {return this->_origin;}
        
        // Intersection test for line(ray) - infinite plane - intersection.
        virtual float intersect(const Ray& ray, float t_max, int flags) const;

        // Distance from a 3D-point P to the closest point on the plane.
        virtual float distance(const glm::vec3& p) const;
    };
};

#endif // __rt_primitive_h__