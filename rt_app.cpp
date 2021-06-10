#include "rt_app.h"

#include <omp.h>

RT_Application::RT_Application(void)
{
    this->light =
    {
        {-1.0f, 0.5f, 0.0f},
        {7.0f, 7.0f, 7.0f}
    };

    rt::Sphere spheres[PRIM_COUNT] = 
    {
        rt::Sphere
        (
            {0.0f, 0.0f, 3.0f},
            1.0f,
            {
                {0.0f, 0.0f, 1.0f},
                0.8f,
                0.5f,
                1.0f
            }
        ),
        rt::Sphere
        (
            {3.0f, 0.0f, 3.0f},
            1.0f,
            {
                {0.0f, 1.0f, 0.0f},
                0.8f,
                0.5f,
                1.0f
            }
        ),
        rt::Sphere
        (
            {-1.75f, -1001.0f, 3.0f},
            1000.0f,
            {
                {1.0f, 1.0f, 1.0f},
                0.7f,
                0.0f,
                1.0f
            }
        )
    };

    rt::Framebuffer fbo;
    fbo.width = SCR_WIDTH;
    fbo.height = SCR_HEIGHT;

    rt::BufferLayoutInfo info;
    info.size = PRIM_COUNT;
    info.first = 0;
    info.last = info.size;

    rt::Buffer buff(info);
    buff.data(0, 3, spheres);

    this->set_num_threads(omp_get_max_threads() * 2);
    this->set_framebuffer(fbo);
    this->clear_color(0.0f, 0.0f, 0.0f);
    this->draw_buffer(buff);
}

RT_Application::~RT_Application(void)
{
    // not used dtor of the super class handles everything (in this application)
}

float RT_Application::sdf(const glm::vec3& p, float t_max, const rt::Primitive** hit_prim)
{
    float d = t_max;                                        // Use maximum length of the ray if there is no object within that range.
    const size_t n_buff = this->rt_geometry_buffer_count();

    for(size_t b = 0; b < n_buff; b++)
    {
        const rt::Primitive * const * map = this->rt_geometry()[b].map_rdonly();
        const size_t size = this->rt_geometry()[b].info().size;

        for(size_t i = 0; i < size; i++)                    // Iterate through the primitives and find the closest one.
        {
            if(map[i] != nullptr)
            {
                const float cur_d = map[i]->distance(p);
                if(cur_d < d && cur_d < t_max)              // dont update if current distance exeeds the maximum length of the ray (outside the render distance)
                {
                    d = cur_d;                              // update the closest length if the current length is maller than the current closest length
                    if(hit_prim != nullptr)                 // set the id of the closest sphere
                        *hit_prim = map[i];
                }
            }
        }
    }
    return d;
}

float RT_Application::shadow(const rt::Ray& shadow_ray, float t_max, float softness)
{
    static constexpr int MAX_ITERATIONS = 128;                              // Maximum iterations of the ray marging process.

    float t = 0.15f;                                                        // A small bias to minimize the render artefacts.
    float res = 1.0f;                                                       // amount of light hitting the surface, 1.0 = 100%, 0.0 = 0%

    int iter_cntr = 0;                                                      // Current iteration
    while(iter_cntr++ < MAX_ITERATIONS && t < t_max)                        // Also end the loop if the ray-length is the maximum length of longer
    {
        glm::vec3 p = shadow_ray.origin + t * shadow_ray.direction;         // P = O + t*D    
        float d = this->sdf(p, t_max, nullptr);                             // get disance to the closest point

        if(d <= 0.0001f)                                                    // 0.0001 or smaller counts as intersecion to accelerate the ray-marging process
            return 0.0f;                                                    // If the ray hits an object directly, there will be no light
        t += d;
        /*  For penumbra calculation (soft part of the shadow) we need the closest distance to an object along the ray. 
            That the resulting value for the shadow is in the range of 0 to 1, we devide the actual distance by the length of the ray.
            As we want the smallest result of that ratio, we use the min function in combination with the preveous result ('res'). 
            The softness value is multiplied with 'd', so the size of the penumbra can be adjusted. */
        res = glm::min(res, softness * d / t);
    }
    return res;
}

glm::vec3 RT_Application::ray_generation_shader(float x, float y)
{
    x *= this->rt_ratio();

    glm::vec3 origin    = {0.0f, 0.0f, -5.0f};                                              // origin of the camera
    glm::vec3 look_at   = {2.0f, 0.0f, 0.0f};                                               // direction/point the camere is looking at

    glm::vec3 cam_z     = glm::normalize(look_at - origin);                                 // z-direction of the camera
    glm::vec3 cam_x     = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cam_z));   // x-diretion of the camera
    glm::vec3 cam_y     = glm::cross(cam_z, cam_x);                                         // y-direction of the camera

    // generate ray
    rt::Ray ray;
    ray.origin = origin;
    ray.direction = glm::normalize(x * cam_x + y * cam_y + 1.5f * cam_z);                   // rotated intersection with the image plane

    // render the image in hdr
    glm::vec3 hdr_color = trace_ray(ray, 5, 100.0f);                                        // begin ray-tracing process                              
    glm::vec3 ldr_color = hdr_color / (hdr_color + glm::vec3(1.0f));                        // convert to ldr

    return ldr_color;
}

glm::vec3 RT_Application::closest_hit_shader(const rt::Ray& ray, int recursion, float t, float t_max, const rt::Primitive* hit)
{
    glm::vec3 out_color;                                                                    // output/result color

    const rt::Sphere* hit_sphere = dynamic_cast<const rt::Sphere*>(hit);
    if(hit_sphere == nullptr)
        return {0.0f, 0.0f, 0.0f};

    glm::vec3 I = ray.origin + (t + 0.0001f) * ray.direction;                               // intersection point
    glm::vec3 N = glm::normalize(I - hit_sphere->center());                                 // surface's normal vector at the intersection point

    // combute absorption
    glm::vec3 light_intensity = rt::light(this->light, hit_sphere->material(), -ray.direction, N);   // combute surface's light intensity

    // combute shadow
    rt::Ray shadow_ray = {I, this->light.direction};
    float shadow_value = 0.0f;
    if(glm::dot(this->light.direction, N) > 0.0f)
        shadow_value = this->shadow(shadow_ray, t_max, 10.0f);

    glm::vec3 absorb_light = hit_sphere->material().albedo * 0.3f + light_intensity * shadow_value; // final aborbed light intensity

    // combute reflection
    rt::Ray reflect_ray     = {I, glm::normalize(glm::reflect(ray.direction, N))};      // let the incoming light ray reflect around the surface's normal vector
    glm::vec3 reflect_light = trace_ray(reflect_ray, recursion-1, t_max);               // trace the reflected ray

    // combute refraction
    static constexpr float n_air        = 1.0f;                                         // refraction index of air
    static constexpr float n_glass      = 1.52f;                                        // refraction index of glass
    static constexpr float n_ratio      = n_air / n_glass;
    static constexpr float n_ratio_inv  = n_glass / n_air;

    rt::Ray refract_ray = {I, glm::refract(ray.direction, N, n_ratio)};                 // let the incoming light ray refract at the entrance of the sphere

    float t_back        = hit->intersect(refract_ray, t_max, RT_INTERSECTION_CONSIDER_INSIDE);      // back-side intersecion
    glm::vec3 i_back    = refract_ray.origin + (t_back - 0.0001f) * refract_ray.direction;          // intersection point of the back-side
    glm::vec3 n_back    = glm::normalize(hit_sphere->center() - i_back);                            // surface's normal vector at the back-side intersection point

    refract_ray.direction   = glm::refract(refract_ray.direction, n_back, n_ratio_inv); // let the light ray refract a second time at the outrance of the sphere
    refract_ray.origin      = i_back;
    glm::vec3 refract_light = trace_ray(refract_ray, recursion-1, t_max);               // trace the refracted ray

    // calculate final color result -> A + R + T = 100%
    const float absorb  = hit_sphere->material().roughness;
    const float reflect = 1.0f - hit_sphere->material().roughness;

    out_color = hit_sphere->material().apha * (absorb * absorb_light + reflect * reflect_light) + (1.0f - hit_sphere->material().apha) * refract_light;
    return out_color;
}

glm::vec3 RT_Application::miss_shader(const rt::Ray& ray, int recursuon, float t_max)
{
    return {0.0f, 0.0f, 0.0f};
}

void RT_Application::app_run(void)
{
    this->run();
}

const rt::Color3ui8* RT_Application::fetch_pixels(void) noexcept
{
    return this->get_framebuffer().buff;
}