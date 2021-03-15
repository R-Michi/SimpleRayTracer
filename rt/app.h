#ifndef __rt_application_h__
#define __rt_application_h__

#include "rt_types.h"
#include "primitive.h"

namespace rt
{
    class RayTracer
    {
    private:
        glm::vec3 main_shader(void);

    protected:
        glm::vec3 trace_ray(const Ray& ray, int recursions);

        virtual glm::vec3 ray_generation_shader(float x, float y) = 0;
        virtual glm::vec3 closest_hit_shader(const Ray& ray, int recursion, float t, float t_max, const Primitive* hit) = 0;
        virtual glm::vec3 miss_shader(const Ray& ray, int recursuon, float t_max) = 0;
        
    public:
        RayTracer(void);
        virtual ~RayTracer(void);
    };
}

#endif //__rt_application_h__