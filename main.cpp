// include std libraries
#include <chrono>   // for time measurement
#include <cstdio>   // for printf
#include <cstdint>
#include <cinttypes>
#undef __STRICT_ANSI__
#include <cmath>
#include <omp.h>

// include glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// incldue stbmaster
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

// include glc
#include <GL/glc.h>

#include "rt/primitive.h"
#include "rt/light.h"

#define SCR_WIDTH       (960 * 4)
#define SCR_HEIGHT      (540 * 4)
#define SCR_ASPECT      ((float)SCR_WIDTH / (float)SCR_HEIGHT)
#define SCR_PIXELS      (SCR_WIDTH * SCR_HEIGHT)
#define PRIM_COUNT      3
#define RT_RECURSIONS   5
#define RAND_SEED       0

#define INTERSECTION_CONSIDER_INSIDE    0x1

#define atXY(x, y, stride) (y * stride + x)

/* FUNCTION DECLARATION */

// returns a random float in the range of 0 and 1
float random_float_01(void);

/**
 *  Cleares the image with a RGB color.
 *  @param buff -> Pixel buffer
 *  @param r -> Red value
 *  @param g -> Green value
 *  @param b -> Blue value
 */
void clear_color(rt::Color3ui8* buff, float r, float g, float b);

/**
 *  Tests if a ray intersects with a sphere in the scene.
 *  @param spheres -> Spheres to be checked for intersection.
 *  @param n_spheres -> The number of the spheres.
 *  @param ray -> The ray that is tested if it intersects with a sphere.
 *  @param t_max -> The maximum length of the ray. Similar to the render distance.
 *  @param flags -> Can change the way how intersection is calculated.
 *  @return @param primitive_id -> The id of the sphere (primitive) that the ray intersected with.
 *  @return -> The length of the ray-origin to the closest intersection point.
 */
float intersection_sphere(const rt::Sphere* const * spheres, size_t n_spheres, const rt::Ray& ray, float t_max, const rt::Sphere** prim, int flags);

/**
 *  Combutes how much an object is in shadow.
 *  @param spheres -> Spheres (primitives / objects) of the scene.
 *  @param n_spheres -> The number of the spheres.
 *  @param shadow_ray -> The ray that is traced from a point in the space to the light source.
 *  @param t_max -> The maximum length of the ray.
 *  @param softness -> Defines how soft is the shadow (size of the penumbra), whereby 0 is infinite soft.
 *  @return -> A value between 0 and 1 that determines how much the point is in shadow.
 */
float shadow(const rt::Sphere* const * spheres, size_t n_spheres, const rt::Ray& shadow_ray, float t_max, float softness);

/**
 *  Calculates the distance to the closest sphere (primitive / object) from a given point P.
 *  @param spheres -> Spheres (primitives / objects) of the scene.
 *  @param n_spheres -> The number of the spheres.
 *  @param p -> The point from where the closest distance is calculated.
 *  @param t_max -> The maximum length of the ray.
 *  @return @param primitive_id -> The id of the closest sphere (primitive).
 *  @return -> Distance to the closest sphere (primitive / object). 
 */
float sdf(const rt::Sphere* const * spheres, size_t n_spheres, const glm::vec3& p, float t_max, int* primitive_id);

/**
 *  Begins the ray-tracing process.
 *  @param ray -> Ray to be traced.
 *  @param recursions -> The number of recursions to be called.
 *  @return -> The light intensity (color) of the intersecion.
 */
glm::vec3 trace_ray(const rt::Ray& ray, int recursions);

/**
 *  This shader gets called if there is an intersection with a sphere.
 *  @param ray -> The ray that was traced to that intersction.
 *  @param recursion -> The actual recursion of the ray-tracing process.
 *  @param t -> The length of the vector from origin to the intersection.
 *  @param t_max -> The maximum length of any ray.
 *  @param primitive_id -> The id of the primitive (sphere) that was hit by the ray.
 *  @return -> A color-value of the current intersection point.
 */
glm::vec3 closest_hit_shader(const rt::Ray& ray, int recursion, float t, float t_max, const rt::Sphere* hit_sphere);

/**
 *  This shader gets called if there is no intersection with any object in the scene.
 *  @param ray -> The ray that was traced into the void.
 *  @param recursion -> The actual recursion of the ray-tracing process.
 *  @param t_max -> The maximum length of any ray.
 *  @return -> A color-value if the ray misses.
 */
glm::vec3 miss_shader(const rt::Ray& ray, int recursion, float t_max);

/**
 *  The shader computes the color of a pixel at position x, y.
 *  @param x -> X coordinate in normalized device coordinates.
 *  @param y -> Y coordinate in normalized device coordinates.
 *  @return -> Color of the pixel at X, Y.
 */
glm::vec3 shader(float x, float y);

/* GLOBAL VARIABLE DEFINITION */

// some spheres
rt::Sphere* spheres[PRIM_COUNT]
{
    new rt::Sphere
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
    new rt::Sphere
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
    new rt::Sphere
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

rt::Light _light = 
{
    {-1.0f, 0.5f, 0.0f},
    {7.0f, 7.0f, 7.0f}
};

/* FUNCTION IMPLEMANTATION */

float random_float_01(void)
{
    static bool first = true;
    if(first)
    {
        srand(RAND_SEED);
        first = false;
    }
    return (rand() % 32768) / 32768.0f;
}

void clear_color(rt::Color3ui8* buff, float r, float g, float b)
{
    for(size_t i=0; i < SCR_PIXELS; i++)    // Iterate through every pixel...
        buff[i] = glm::vec3(r, g, b);       // ... and set it the the given color.
}

float intersection_sphere(const rt::Sphere* const * spheres, size_t n_spheres, const rt::Ray& ray, float t_max, const rt::Sphere** prim, int flags)
{
    float t = t_max;                               // distance from the origin to the closest hitpoint
    for(size_t i=0; i<n_spheres; i++)                       // iterate through all spheres
    {
        /*  The intersecion point is a quadratic function: ax^2 + bx + c = 0
            These following lines calculate the parameter a, b, c of that function */
        glm::vec3 oc = ray.origin - spheres[i]->center();  
        const float a = 1;                                  // "a" is the direction of the ray squared. Since the ray-direction is a unit vector, squaring it will still result 1.
        const float b = 2 * glm::dot(ray.direction, oc);
        const float c = glm::dot(oc, oc) - spheres[i]->radius() * spheres[i]->radius();
        const float delta = b*b - 4*a*c;

        // if ray intersects with the sphere
        if(delta >= 0.0f)
        {
            // calculate both results of the quadratic equation
            float t0, t1;                                   // both resuls of the quadratic function
            if(delta == 0.0f)                               // just an optimization if both solutions are the same
                t0 = t1 = -0.5f * (b / a);
            else                                            // if they are not the same, use the standard formula
            {
                t0 = (-b - sqrt(delta)) / (2 * a);
                t1 = (-b + sqrt(delta)) / (2 * a);
            }

            /** 
             *  The intersection test can ba made with fewer lines of code.
             *  However, my aim was to minimize the number of comparisons needed to get the final result.
             * 
             *  The first check is if the sphere is within the render distance, if not there are no more comparions needed.
             *  The second check is if the sphere in front of us.
             *  The third check is if we are within the sphere.
             * 
             *  Note: The order of the second and third check is important, since there are more objects in front of us, than within us.
             *  This will result in less comparisons in total.
             * 
             *  Also important is that we are testing the closest intersection t0 at first at this determines if we are in front of or
             *  within the sphere. This also results in less comparisons.
             */
            if(t0 < t_max)
            {
                if(t0 >= 0.0f && t1 >= 0.0f && t0 < t)
                {
                    if(prim != nullptr)
                        *prim = spheres[i];
                    t = t0;
                }
                else if(flags & INTERSECTION_CONSIDER_INSIDE && t0 < 0.0f && t1 >= 0.0f && t1 < t)
                {
                    if(prim != nullptr)
                        *prim = spheres[i];
                    t = t1;
                }
            }
        }
    }
    return t;
}

float intersection(const rt::Sphere* const * spheres, size_t n_spheres, const rt::Ray& ray, float t_max, const rt::Sphere** prim, int flags)
{
    float t = t_max;
    float t_cur;
    for(size_t i=0; i<n_spheres; i++)
    {
        t_cur = spheres[i]->intersect(ray, t_max, flags);
        if(t_cur < t)
        {
            if(prim != nullptr)
                *prim = spheres[i];
            t = t_cur;
        }
    }
    return t;
}

float shadow(const rt::Sphere* const * spheres, size_t n_spheres, const rt::Ray& shadow_ray, float t_max, float softness)
{
    float t = 0.15f;                                                        // A small bias to minimize the render artefacts.
    float res = 1.0f;                                                       // amount of light hitting the surface, 1.0 = 100%, 0.0 = 0%

    static constexpr int MAX_ITERATIONS = 128;                              // Maximum iterations of the ray marging process.
    int iter_cntr = 0;                                                      // Current iteration
    while(iter_cntr++ < MAX_ITERATIONS && t < t_max)                        // Also end the loop if the ray-length is the maximum length of longer
    {
        glm::vec3 p = shadow_ray.origin + t * shadow_ray.direction;         // P = O + t*D    
        float d = sdf(spheres, n_spheres, p, t_max, nullptr);               // get disance to the closest point
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

float sdf(const rt::Sphere* const * spheres, size_t n_spheres, const glm::vec3& p, float t_max, int* primitive_id)
{
    float d = t_max;                       // Use maximum length of the ray if there is no object within that range.
    for(size_t i=0; i<n_spheres; i++)             // Iterate through the spheres and find the closest one.
    {
        const float cur_d = glm::length(spheres[i]->center() - p) - spheres[i]->radius();
        if(cur_d < t_max && cur_d < d)     // dont update if current distance exeeds the maximum length of the ray (outside the render distance)
        {
            d = cur_d;                              // update the closest length if the current length is maller than the current closest length
            if(primitive_id != nullptr)             // set the id of the closest sphere
                *primitive_id = i;
        }
    }
    return d;
}

glm::vec3 trace_ray(const rt::Ray& ray, int recursions)
{
    static constexpr float RAY_MAX_LENGTH = 100.0f;     // Maximum length of any ray (render distance / z-far)
    glm::vec3 out_color = {0.0f, 0.0f, 0.0f};           // initialize color of the current ray to black that is has no contribution

    if(recursions == 0)                                 // all recursions ran through, so we can end the ray-tracing process
        return out_color;

    const rt::Sphere* hit_sphere = nullptr;             // The id of the primitive (sphere) that was hit
    float t = intersection(spheres, PRIM_COUNT, ray, 100.0f, &hit_sphere, 0);

    if(t < RAY_MAX_LENGTH)                              // if there is an intersection
        out_color = closest_hit_shader(ray, recursions, t, RAY_MAX_LENGTH, hit_sphere);
    else                                                // if there is no intersection
        out_color = miss_shader(ray, recursions, RAY_MAX_LENGTH);

    // trace the next recursion
    return out_color;
}

glm::vec3 closest_hit_shader(const rt::Ray& ray, int recursion, float t, float t_max, const rt::Sphere* hit_sphere)
{
    glm::vec3 out_color;                                                                    // output/result color

    glm::vec3 I = ray.origin + (t + 0.0001f) * ray.direction;                               // intersection point
    glm::vec3 N = glm::normalize(I - hit_sphere->center());                                 // surface's normal vector at the intersection point

    // combute absorption
    glm::vec3 light_intensity = light(_light, hit_sphere->material(), -ray.direction, N);   // combute surface's light intensity

    // combute shadow
    rt::Ray shadow_ray = {I, _light.direction};
    float shadow_value = 0.0f;
    if(glm::dot(_light.direction, N) > 0.0f)
        shadow_value = shadow(spheres, PRIM_COUNT, shadow_ray, t_max, 10.0f);

    glm::vec3 absorb_light = hit_sphere->material().albedo * 0.3f + light_intensity * shadow_value; // final aborbed light intensity

    // combute reflection
    rt::Ray reflect_ray     = {I, glm::normalize(glm::reflect(ray.direction, N))};      // let the incoming light ray reflect around the surface's normal vector
    glm::vec3 reflect_light = trace_ray(reflect_ray, recursion-1);                      // trace the reflected ray

    // combute refraction
    static constexpr float n_air        = 1.0f;                                         // refraction index of air
    static constexpr float n_glass      = 1.52f;                                        // refraction index of glass
    static constexpr float n_ratio      = n_air / n_glass;
    static constexpr float n_ratio_inv  = n_glass / n_air;

    rt::Ray refract_ray = {I, glm::refract(ray.direction, N, n_ratio)};                 // let the incoming light ray refract at the entrance of the sphere

    float t_back        = intersection(&hit_sphere, 1, refract_ray, t_max, nullptr, RT_INTERSECTION_CONSIDER_INSIDE);       // back-side intersecion
    glm::vec3 i_back    = refract_ray.origin + (t_back - 0.0001f) * refract_ray.direction;                                  // intersection point of the back-side
    glm::vec3 n_back    = glm::normalize(hit_sphere->center() - i_back);                                                    // surface's normal vector at the back-side intersection point

    refract_ray.direction   = glm::refract(refract_ray.direction, n_back, n_ratio_inv); // let the light ray refract a second time at the outrance of the sphere
    refract_ray.origin      = i_back;
    glm::vec3 refract_light = trace_ray(refract_ray, recursion-1);                      // trace the refracted ray

    // calculate final color result -> A + R + T = 100%
    const float absorb  = hit_sphere->material().roughness;
    const float reflect = 1.0f - hit_sphere->material().roughness;

    out_color = hit_sphere->material().apha * (absorb * absorb_light + reflect * reflect_light) + (1.0f - hit_sphere->material().apha) * refract_light;
    return out_color;
}

glm::vec3 miss_shader(const rt::Ray& ray, int recursion, float t_max)
{
    return {0.0f, 0.0f, 0.0f};
}

glm::vec3 shader(float x, float y)
{
    x *= SCR_ASPECT;

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
    glm::vec3 hdr_color = trace_ray(ray, RT_RECURSIONS);                // begin ray-tracing process                              
    glm::vec3 ldr_color = hdr_color / (hdr_color + glm::vec3(1.0f));    // convert to ldr

    return ldr_color;
}

int main()
{
    rt::Color3ui8* pixelbuff = new rt::Color3ui8[SCR_PIXELS];           // create pixel buffer for image
    clear_color(pixelbuff, 0.0f, 0.0f, 0.0f);                   // set the image to a default color

    omp_set_num_threads(omp_get_max_threads());                 // use all aviable processors for rendering
    printf("Using %d processors.\n", omp_get_max_threads());

    std::chrono::time_point<std::chrono::system_clock> t0_render = std::chrono::high_resolution_clock::now();   // time before rendering
    #pragma omp parallel for
    for(size_t y=0; y<SCR_HEIGHT; y++)                          // iterate through the image
    {
        for(size_t x=0; x<SCR_WIDTH; x++)
        {
            float ndc_x = gl::convert::from_pixels_pos_x(x, SCR_WIDTH);
            float ndc_y = gl::convert::from_pixels_pos_y(y, SCR_HEIGHT);
            pixelbuff[atXY(x, y, SCR_WIDTH)] = shader(ndc_x, ndc_y);
        }
    }
    std::chrono::time_point<std::chrono::system_clock> t1_render = std::chrono::high_resolution_clock::now();   // time after rendering
    int64_t t_render = std::chrono::duration_cast<std::chrono::milliseconds>(t1_render - t0_render).count();    // get time duration of image rendering operation
    printf("Rendering time: %" PRId64 "ms\n", t_render);

    std::chrono::time_point<std::chrono::system_clock> t0_out = std::chrono::high_resolution_clock::now();      // time before writing
    stbi_write_png("rt_output.png", SCR_WIDTH, SCR_HEIGHT, 3, pixelbuff, SCR_WIDTH * sizeof(rt::Color3ui8));        // print image
    std::chrono::time_point<std::chrono::system_clock> t1_out = std::chrono::high_resolution_clock::now();      // time after writing
    int64_t t_out = std::chrono::duration_cast<std::chrono::milliseconds>(t1_out - t0_out).count();             // get time duration of image writing operation
    printf("Writing time: %" PRId64 "\n", t_out);

    delete(pixelbuff);  // delete pixel buffer
    return 0;
}