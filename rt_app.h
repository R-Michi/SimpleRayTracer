#ifndef __rt_app_h__
#define __rt_app_h__

#include "rt/ray_tracing.h"

#include <cmath>

class RT_Application : protected rt::RayTracer
{
private:
    rt::Light light;
    rt::Cubemap environment;

    /**
     *  Calculates the distance to the closest sphere (primitive / object) from a given point P.
     *  @param prim -> Primitives to be checked for intersection.
     *  @param n_spheres -> The number of the spheres.
     *  @param p -> The point from where the closest distance is calculated.
     *  @param t_max -> The maximum length of the ray.
     *  @return @param hit_prim -> The primitive that the ray intersected with.
     *  @return -> Distance to the closest sphere (primitive / object). 
     */
    float sdf(const glm::vec3& p, float t_max, const rt::Primitive** hit_prim);

    /**
     *  Combutes how much an object is in shadow.
     *  @param prim -> Primitives to be checked for intersection.
     *  @param n_spheres -> The number of the spheres.
     *  @param shadow_ray -> The ray that is traced from a point in the space to  the light source.
     *  @param t_max -> The maximum length of the ray.
     *  @param softness -> Defines how soft is the shadow (size of the penumbra), whereby 0 is infinite soft.
     *  @return -> A value between 0 and 1 that determines how much the point is in shadow.
     */
    float shadow(const rt::Ray& shadow_ray, float t_max, float softness);

protected:
    glm::vec3 ray_generation_shader(float x, float y);
    glm::vec3 closest_hit_shader(const rt::Ray& ray, int recursion, float t, float t_max, const rt::Primitive* hit);
    glm::vec3 miss_shader(const rt::Ray& ray, int recursuon, float t_max);

public:
    // constants
    static constexpr int32_t SCR_WIDTH      = 960 * 4;
    static constexpr int32_t SCR_HEIGHT     = 540 * 4;
    static constexpr size_t PRIM_COUNT      = 3;
    static constexpr size_t RT_RECURSIONS   = 5;

    RT_Application(void);
    virtual ~RT_Application(void);

    void app_run(void);
    const rt::Color3ui8* fetch_pixels(void) noexcept;
};

#endif //__rt_app_h__