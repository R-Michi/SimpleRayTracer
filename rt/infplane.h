#ifndef __rt_inf_plane_h__
#define __rt_inf_plane_h__

#include "primitive.h"

namespace rt
{
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

        /// @return -> A dynamic clone of the infinite plane.
        virtual Primitive* clone_dynamic(void)
        {return new InfPlane(*this);}

        /// @return -> Size of the current infinite plane.
        virtual size_t get_sizeof(void)
        {return sizeof(InfPlane);}
    };
}

#endif //__rt_inf_plane_h__