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

// include light
#include "light.hpp"

#include <windows.h>

#define SCR_WIDTH   (960 * 4)
#define SCR_HEIGHT  (540 * 4)
#define SCR_ASPECT  ((float)SCR_WIDTH / (float)SCR_HEIGHT)
#define SCR_PIXELS  (SCR_WIDTH * SCR_HEIGHT)
#define NUM_SPHERES 2
#define RAND_SEED   1845

#define atXY(x, y, stride) (y * stride + x)

/* TYPE DEFINITION */

struct Color3ui8
{
    uint8_t r, g, b;

    Color3ui8& operator= (const Color3ui8& c)
    {
        this->r = c.r;
        this->g = c.g;
        this->b = c.b;
        return *this;
    }

    Color3ui8& operator= (const glm::vec3& c)
    {
        this->r = (c.r >= 0.0f && c.r <= 1.0f) ? 255 * c.r : ((c.r > 1.0f) ? 255 : 0);
        this->g = (c.g >= 0.0f && c.g <= 1.0f) ? 255 * c.g : ((c.g > 1.0f) ? 255 : 0);
        this->b = (c.b >= 0.0f && c.r <= 1.0f) ? 255 * c.b : ((c.b > 1.0f) ? 255 : 0);
        return *this;
    }
};

struct Ray
{
    glm::vec3 direction;
    glm::vec3 origin;
};

struct Sphere
{
    Material mat;
    glm::vec3 center;
    float radius;
};

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
void clear_color(Color3ui8* buff, float r, float g, float b);

/**
 *  Tests if a ray intersects with a sphere in the scene.
 *  @param spheres -> Spheres to be checked for intersection.
 *  @param n_spheres -> The number of the spheres.
 *  @param ray -> The ray that is tested if it intersects with a sphere.
 *  @param ray_max_length -> The maximum length of the ray. Similar to the render distance.
 *  @return @param primitive_id -> The id of the sphere (primitive) that the ray intersected with.
 *  @return -> The length of the ray-origin to the closest intersection point.
 */
float intersection_sphere(const Sphere* spheres, size_t n_spheres, const Ray& ray, float ray_max_length, int* primitive_id);

/**
 *  Combutes how much an object is in shadow.
 *  @param spheres -> Spheres (primitives / objects) of the scene.
 *  @param n_spheres -> The number of the spheres.
 *  @param shadow_ray -> The ray that is traced from a point in the space to the light source.
 *  @param ray_max_length -> The maximum length of the ray.
 *  @param softness -> Defines how soft is the shadow (size of the penumbra), whereby 0 is infinite soft.
 *  @return -> A value between 0 and 1 that determines how much the point is in shadow.
 */
float shadow(const Sphere* spheres, size_t n_spheres, const Ray& shadow_ray, float ray_max_length, float softness);

/**
 *  Calculates the distance to the closest sphere (primitive / object) from a given point P.
 *  @param spheres -> Spheres (primitives / objects) of the scene.
 *  @param n_spheres -> The number of the spheres.
 *  @param p -> The point from where the closest distance is calculated.
 *  @param ray_max_legth -> The maximum length of the ray.
 *  @return @param primitive_id -> The id of the closest sphere (primitive).
 *  @return -> Distance to the closest sphere (primitive / object). 
 */
float sdf(const Sphere* spheres, size_t n_spheres, const glm::vec3& p, float ray_max_length, int* primitive_id);

/**
 *  Function to define the output color of the raytracing iteration
 *  @param cc -> color of the current iteration
 *  @param pc -> color of the previous iteration
 *  @param old_ray -> the ray used for the current iteration
 *  @param new_ray -> the ray used for the next iteration
 *  @param recursion -> number of the current iteration of the recursion
 *  @param primitive_id -> id of the intersected primitive
 *  @return -> Resulting light intensity of the ray. (Resulting color)
 */
glm::vec3 rt_result_qery(const glm::vec3& cc, const glm::vec3& pc, const Ray& old_ray, const Ray& new_ray, int recursion, int primitive_id);

/**
 *  Begins the ray-tracing process.
 *  @param ray -> Ray to be traced.
 *  @param recursions -> The number of recursions to be called.
 *  @return -> The light intensity (color) of the intersecion.
 */
glm::vec3 trace_ray(const Ray& ray, int recursions);

/**
 *  The shader computes the color of a pixel at position x, y.
 *  @param x -> X coordinate in normalized device coordinates.
 *  @param y -> Y coordinate in normalized device coordinates.
 *  @return -> Color of the pixel at X, Y.
 */
glm::vec3 shader(float x, float y);

/* GLOBAL VARIABLE DEFINITION */

// some spheres
Sphere spheres[NUM_SPHERES] = 
{
    {
        {
            {0.0f, 1.0f, 0.0f},
            0.8f,
            0.5f
        },
        {-1.75f, 0.0f, 3.0f},
        1.0f
    },
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.7f,
            0.0f
        },
        {-1.75f, -1001.0f, 3.0f},
        1000.0f
    }
};

#if 0
Sphere spheres[NUM_SPHERES] = 
{
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.2f,
            0.8f
        },
        {-1.1f, 0.0f, 2.0f},
        1.0f
    },
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.2,
            0.8
        },
        {1.1f, 0.0f, 2.0f},
        1.0f
    },
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.2,
            0.8
        },
        {0.0f, 1.8f, 2.0f},
        1.0f
    },
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.2,
            0.8
        },
        {0.0f, -1.8f, 2.0f},
        1.0f
    },
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.2,
            0.8
        },
        {0.0f, 0.0f, 3.8f},
        1.0f
    },
    {
        {
            {1.0f, 1.0f, 1.0f},
            0.2,
            0.8
        },
        {0.0f, 0.0f, 0.2f},
        1.0f
    }
    
};
#endif

#if 0
Sphere spheres[NUM_SPHERES] = 
{
    {
        {
            {0.0f, 1.0f, 1.0f},
            0.3f,
            0.0f
        },
        {0.0f, 0.0f, 4.0f},
        1.0f
    },
    {
        {
            {1.0f, 0.0f, 0.0f},
            0.7,
            0.3
        },
        {2.0f, 1.0f, 7.0f},
        0.5f
    },
    {
        {
            {0.0f, 0.0f, 1.0f},
            0.3f,
            0.2f
        },
        {-1.0f, 2.0f, 5.0f},
        0.7f,
    },
    {
        {
            {0.0f, 1.0f, 0.0f},
            0.2f,
            0.8f
        },
        {0.5f, -0.5f, 2.0f},
        0.3f
    },
    {
        {
            {1.0f, 1.0f, 0.0f},
            0.5f,
            0.5f
        },
        {-7.0f, -3.5f, 10.0f},
        2.5f
    }
};
#endif

Light _light = 
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

void clear_color(Color3ui8* buff, float r, float g, float b)
{
    for(size_t i=0; i < SCR_PIXELS; i++)    // Iterate through every pixel...
        buff[i] = glm::vec3(r, g, b);       // ... and set it the the given color.
}

float intersection_sphere(const Sphere* spheres, size_t n_spheres, const Ray& ray, float ray_max_length, int* primitive_id)
{
    float t = ray_max_length;                               // distance from the origin to the closest hitpoint
    for(size_t i=0; i<n_spheres; i++)                     // iterate through all spheres
    {
        /*  The intersecion point is a quadratic function: ax^2 + bx + c = 0
            These following lines calculate the parameter a, b, c of that function */
        glm::vec3 oc = ray.origin - spheres[i].center;  
        const float a = 1;                                  // "a" is the direction of the ray squared. Since the ray-direction is a unit vector, squaring it will still result 1.
        const float b = 2 * glm::dot(ray.direction, oc);
        const float c = glm::dot(oc, oc) - spheres[i].radius * spheres[i].radius;
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
                t0 = (-b + sqrt(delta)) / (2 * a);
                t1 = (-b - sqrt(delta)) / (2 * a);
            }

            if(t1 < t0)                                     // make t0 always the closest hitpoint
                std::swap(t0, t1);

            bool valid = true;
            if(t0 < 0.0f || t1 < 0.0f)      valid = false;  // not valid if sphere is behind or in the origin
            else if(t0 > ray_max_length)    valid = false;  // not valid if sphere is outside the render distance

            // update closest hitpoint if result is valid and current hitpoint is closer than the current closest hitpoint
            if(valid && t0 < t)
            {
                if(primitive_id != nullptr)
                    *primitive_id = i;
                t = t0;
            }
        }
    }
    return t;
}

float shadow(const Sphere* spheres, size_t n_spheres, const Ray& shadow_ray, float ray_max_length, float softness)
{
    float t = 0.15f;                                                        // A small bias to minimize the render artefacts.
    float res = 1.0f;                                                       // amount of light hitting the surface, 1.0 = 100%, 0.0 = 0%

    static constexpr int MAX_ITERATIONS = 128;                              // Maximum iterations of the ray marging process.
    int iter_cntr = 0;                                                      // Current iteration
    while(iter_cntr++ < MAX_ITERATIONS && t < ray_max_length)               // Also end the loop if the ray-length is the maximum length of longer
    {
        glm::vec3 p = shadow_ray.origin + t * shadow_ray.direction;         // P = O + t*D    
        float d = sdf(spheres, n_spheres, p, ray_max_length, nullptr);    // get disance to the closest point
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

float sdf(const Sphere* spheres, size_t n_spheres, const glm::vec3& p, float ray_max_length, int* primitive_id)
{
    float d = ray_max_length;                       // Use maximum length of the ray if there is no object within that range.
    for(size_t i=0; i<n_spheres; i++)             // Iterate through the spheres and find the closest one.
    {
        const float cur_d = glm::length(spheres[i].center - p) - spheres[i].radius;
        if(cur_d < ray_max_length && cur_d < d)     // dont update if current distance exeeds the maximum length of the ray (outside the render distance)
        {
            d = cur_d;                              // update the closest length if the current length is maller than the current closest length
            if(primitive_id != nullptr)             // set the id of the closest sphere
                *primitive_id = i;
        }
    }
    return d;
}

glm::vec3 rt_result_qery(const glm::vec3& cc, const glm::vec3& pc, const Ray& old_ray, const Ray& new_ray, int recursion, int primitive_id)
{
    const Material& mat = spheres[primitive_id].mat;
    glm::vec3 absorption(mat.roughness);
    glm::vec3 reflection(1.0f - mat.roughness);

    // preveous color is the reflective part
    // current color is the absorbed part
    // COLOR = ABSORPTION + REFLECTION = 100%
    return absorption * cc + reflection * pc;
}

glm::vec3 trace_ray(const Ray& ray, int recursions)
{
    glm::vec3 color = {0.0f, 0.0f, 0.0f};       // initialize color of the current ray to black that is has no contribution
    Ray new_ray = ray;                          // initialize the new ray to the old ray

    if(recursions == 0)                         // all recursions ran through, so we can end the ray-tracing process
        return color;

    constexpr float RAY_MAX_LENGTH = 100.0f;    // Maximum length of any ray (render distance / z-far)

    int hit_sphere = -1;                        // The if of the sphere that was hit
    float t = intersection_sphere(spheres, NUM_SPHERES, ray, RAY_MAX_LENGTH, &hit_sphere);  // find intersection

    if(t == RAY_MAX_LENGTH)                     // if there is no intersection
        return {0.0f, 0.0f, 0.0f};
    else                                        // if there is an intersection
    {
        const Material& cur_mat = spheres[hit_sphere].mat;

        glm::vec3 I = ray.origin + t * ray.direction;                   // intersection point
        glm::vec3 V = -ray.direction;                                   // view vector
        glm::vec3 N = glm::normalize(I - spheres[hit_sphere].center);   // normal vector
        float NdotL = glm::dot(_light.direction, N);                    // cos(phi) of light vector and normal vector

        glm::vec3 intensity = light(_light, cur_mat, V, N);             // get light intensity at current position

        // Combute shadow
        Ray shadow_ray = {glm::normalize(_light.direction), I};
        float shadow_value = 0.0f;
        // An optimization: We dont need to calculate the casted shadow if the angle between the light vector and the surface is greater than 90 degrees.
        if(NdotL > 0.0f)
            shadow_value = shadow(spheres, NUM_SPHERES, shadow_ray, RAY_MAX_LENGTH, 10.0f);
        
        // combute final light intensity of the current point
        color = cur_mat.albedo * 0.3f + intensity * shadow_value;

        // let the ray of the next recursion reflect, so we can combute reflections
        new_ray.origin      = I;
        new_ray.direction   = glm::reflect(ray.direction, N);
    }

    // trace the next recursion
    glm::vec3 prev_color = trace_ray(new_ray, recursions-1);
    return rt_result_qery(color, prev_color, ray, new_ray, recursions, hit_sphere);
}

glm::vec3 shader(float x, float y)
{
    // line-image_plane-intersection
    glm::vec3 origin = {0.0f, 0.0f, 0.0f};
    glm::vec3 point_img_plane = {x, y, 1.5f};   // with the used screen ratio (1.77777) -> 1.5 results in ~100° FOV

    // generate ray
    Ray ray;
    ray.origin = origin;
    ray.direction = glm::normalize(point_img_plane - origin);

    // render the image in hdr
    glm::vec3 hdr_color = trace_ray(ray, 5);    // begin ray-tracing process                              
    glm::vec3 ldr_color = hdr_color / (hdr_color + glm::vec3(1.0f));    // convert to ldr

    return ldr_color;
}

int main()
{
    Color3ui8* pixelbuff = new Color3ui8[SCR_PIXELS];           // create pixel buffer for image
    clear_color(pixelbuff, 0.0f, 0.0f, 0.0f);                   // set the image to a default color

    omp_set_num_threads(omp_get_max_threads());                 // use all aviable processors for rendering
    printf("Using %d processors.\n", omp_get_max_threads());

    std::chrono::time_point<std::chrono::system_clock> t0_render = std::chrono::high_resolution_clock::now();   // time before rendering
    #pragma omp parallel for
    for(size_t y=0; y<SCR_HEIGHT; y++)                          // iterate through the image
    {
        for(size_t x=0; x<SCR_WIDTH; x++)
        {
            float ndc_x = gl::convert::from_pixels_pos_x(x, SCR_WIDTH) * SCR_ASPECT;
            float ndc_y = gl::convert::from_pixels_pos_y(y, SCR_HEIGHT);
            pixelbuff[atXY(x, y, SCR_WIDTH)] = shader(ndc_x, ndc_y);
        }
    }
    std::chrono::time_point<std::chrono::system_clock> t1_render = std::chrono::high_resolution_clock::now();   // time after rendering
    int64_t t_render = std::chrono::duration_cast<std::chrono::milliseconds>(t1_render - t0_render).count();    // get time duration of image rendering operation
    printf("Rendering time: %" PRId64 "ms\n", t_render);

    std::chrono::time_point<std::chrono::system_clock> t0_out = std::chrono::high_resolution_clock::now();      // time before writing
    stbi_write_png("rt_output.png", SCR_WIDTH, SCR_HEIGHT, 3, pixelbuff, SCR_WIDTH * sizeof(Color3ui8));        // print image
    std::chrono::time_point<std::chrono::system_clock> t1_out = std::chrono::high_resolution_clock::now();      // time after writing
    int64_t t_out = std::chrono::duration_cast<std::chrono::milliseconds>(t1_out - t0_out).count();             // get time duration of image writing operation
    printf("Writing time: %" PRId64 "\n", t_out);

    delete(pixelbuff);  // delete pixel buffer
    return 0;
}