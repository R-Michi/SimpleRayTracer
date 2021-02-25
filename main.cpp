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

#define SCR_WIDTH   (960 * 4)
#define SCR_HEIGHT  (540 * 4)
#define SCR_ASPECT  ((float)SCR_WIDTH / (float)SCR_HEIGHT)
#define SCR_PIXELS  (SCR_WIDTH * SCR_HEIGHT)
#define NUM_SPHERES 5

#define atXY(x, y, stride) \
        (y * stride + x)

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
    glm::vec3 color;
    glm::vec3 center;
    float radius;
};

/* FUNCTION DECLARATION */

// Clears the color of the image
// r, g, b represents the clear color
void clear_color(Color3ui8* buff, float r, float g, float b);

// Function finds the intersection of the closest sphere
/// @return distance from the origin to the closest hitpoint
float intersection_sphere(const Sphere* spheres, size_t num_spheres, const Ray& ray, float ray_max_length, int* hit_sphere);

// Begins the ray-tracing process
glm::vec3 trace_ray(const Ray& ray, int recursions);

// The shader computes the color of a pixel at position x, y
glm::vec3 shader(float x, float y);

/* GLOBAL VARIABLE DEFINITION */

Sphere spheres[NUM_SPHERES] = 
{
    {
        {0.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 4.0f},
        1.0f
    },
    {
        {1.0f, 0.0f, 0.0f},
        {2.0f, 1.0f, 7.0f},
        0.5f
    },
    {
        {0.0f, 0.0f, 1.0f},
        {-1.0f, 2.0f, 5.0f},
        0.7f,
    },
    {
        {0.0f, 1.0f, 0.0f},
        {0.5f, -0.5f, 2.0f},
        0.3f
    },
    {
        {1.0f, 1.0f, 0.0f},
        {-7.0f, -3.5f, 10.0f},
        2.5f
    }
};

Light _light = 
{
    {-1.0f, 1.0f, -1.0f},
    {3.0f, 3.0f, 3.0f}
};

/* FUNCTION IMPLEMANTATION */

void clear_color(Color3ui8* buff, float r, float g, float b)
{
    for(size_t i=0; i < SCR_PIXELS; i++)
        buff[i] = glm::vec3(r, g, b);
}

float intersection_sphere(const Sphere* spheres, size_t num_spheres, const Ray& ray, float ray_max_length, int* hit_sphere)
{
    float t = ray_max_length;   // distance from the origin to the closest hitpoint
    for(size_t i=0; i<num_spheres; i++)
    {
        glm::vec3 oc = ray.origin - spheres[i].center;
        const float a = 1;
        const float b = 2 * glm::dot(ray.direction, oc);
        const float c = glm::dot(oc, oc) - spheres[i].radius * spheres[i].radius;
        const float delta = b*b - 4*a*c;

        

        // if ray intersects with the sphere
        if(delta >= 0.0f)
        {
            // calculate both results of the quadratic equation
            float t0, t1;
            if(delta == 0.0f)
                t0 = t1 = -0.5f * (b / a);
            else
            {
                t0 = (-b + sqrt(delta)) / (2 * a);
                t1 = (-b - sqrt(delta)) / (2 * a);
            }

            // make t0 always the closest hitpoint
            if(t1 < t0)
                std::swap(t0, t1);

            bool valid = true;
            if(t0 < 0.0f || t1 < 0.0f)      valid = false;  // not valid if sphere is behind or in the origin
            else if(t0 > ray_max_length)    valid = false;  // not valid if sphere is outside the render distance

            // update closest hitpoint if result is valid and current hitpoint is closer than the current closest hitpoint
            if(valid && t0 < t)
            {
                if(hit_sphere != nullptr)
                    *hit_sphere = i;
                t = t0;
            }
        }
    }
    return t;
}

glm::vec3 trace_ray(const Ray& ray, int recursions)
{
    glm::vec3 color = {0.0f, 0.0f, 0.0f};
    Ray new_ray = ray;

    if(recursions == 0) // all recursions ran through, so we can end the ray-tracing process
        return color;

    constexpr float ray_max_length = 100.0f;

    int hit_sphere;
    float t = intersection_sphere(spheres, NUM_SPHERES, ray, ray_max_length, &hit_sphere);  // find intersection

    if(t == ray_max_length)             // black color if there is no intersection
        return {0.0f, 0.0f, 0.0f};
    else                                // if there is an intersection
    {
        Material material;
        material.albedo     = spheres[hit_sphere].color;
        material.roughness  = 0.3f;
        material.metallic   = 0.0f;

        glm::vec3 I = ray.origin + t * ray.direction;                   // intersection point
        glm::vec3 V = -ray.direction;                                   // view vector
        glm::vec3 N = glm::normalize(I - spheres[hit_sphere].center);   // normal vector

        glm::vec3 intensity = light(_light, material, V, N);
        color = material.albedo * 0.1f + intensity;

        new_ray.origin      = I;
        new_ray.direction   = glm::reflect(ray.direction, N);
    }

    // trace the next recursion
    color += trace_ray(new_ray, --recursions);
    return color;
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
    glm::vec3 hdr_color = trace_ray(ray, 3);    // begin ray-tracing process                              
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