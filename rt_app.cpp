/**
* @file     rt_app.cpp
* @brief    Source file for the ray-tracing test application.
*           In this file I am experimenting with ray tracing.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "rt_app.h"
#include "glc-1.0.0/GL/glc.h"

#include <omp.h>
#include <stdexcept>

#include <iostream>
#include <chrono>

inline uint32_t float2bits(float x)
{
    uint32_t i;
    memcpy(&i, &x, sizeof(float));
    return i;
}

inline float bits2float(uint32_t x)
{
    float f;
    memcpy(&f, &x, sizeof(uint32_t));
    return f;
}

float next_float_up(float x)
{
    // 2 special cases:
    // if x is positive infinity, then the value can't be incremented any further and it is returned unchanged
    // if x is negative 0, change it to positive 0, as negative 0 has a different memory representation
    if (std::isinf(x) && x > 0.0f)
        return x;
    if (x == -0.0f)
        x = 0.0f;

    // advance x to the next higher float
    // this can only be done because of the memory representation of the IEEE754 standard
    uint32_t i = float2bits(x);
    if (x >= 0) ++i;
    else        --i;
    return bits2float(i);
}

float next_float_down(float x)
{
    // is the same as next_float_up but in reverse
    if (std::isinf(x) && x < 0.0f)
        return x;
    if (x == 0.0f)
        x = -0.0f;

    uint32_t i = float2bits(x);
    if (x <= 0) ++i;
    else        --i;
    return bits2float(i);
}

inline glm::vec3 offset_ray_origin(const glm::vec3& p, const glm::vec3& n, const glm::vec3& w)
{
    float d = 2e-4f;
    glm::vec3 offset = d * n;
    if (dot(w, n) < 0)
        offset = -offset;
    glm::vec3 po = p + offset;

    if (offset.x > 0)       po.x = next_float_up(po.x);
    else if (offset.x < 0)  po.x = next_float_down(po.x);
    if (offset.y > 0)       po.y = next_float_up(po.y);
    else if (offset.y < 0)  po.y = next_float_down(po.y);
    if (offset.z > 0)       po.z = next_float_up(po.z);
    else if (offset.z < 0)  po.z = next_float_down(po.z);

    return po;
}


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
            {0.0f, 0.0f, 5.0f},
            1.0f,
            Material(glm::vec3(0.0f, 0.0f, 1.0f), 0.5f, 0.8f, 1.0f)
        ),
        rt::Sphere
        (
            {3.0f, 0.0f, 3.0f},
            1.0f,
            Material(glm::vec3(0.0f, 1.0f, 0.0f), 0.5f, 0.8f, 1.0f)
        ),
        rt::Sphere
        (
            {-1.75f, -1001.0f, 3.0f},
            1000.0f,
            Material(glm::vec3(1.0f, 1.0f, 1.0f), 0.5f, 0.8f, 1.0f)
        )
    };

#if 0
    rt::CubemapCreateInfo cubemap_ci = {};
    cubemap_ci.front    = "../../../assets/skyboxes/front.jpg";
    cubemap_ci.back     = "../../../assets/skyboxes/back.jpg";
    cubemap_ci.top      = "../../../assets/skyboxes/top.jpg";
    cubemap_ci.bottom   = "../../../assets/skyboxes/bottom.jpg";
    cubemap_ci.left     = "../../../assets/skyboxes/left.jpg";
    cubemap_ci.right    = "../../../assets/skyboxes/right.jpg";
    cubemap_ci.filter   = rt::RT_FILTER_LINEAR;

    if (this->environment.load(cubemap_ci) == rt::RT_TEXTURE_ERROR_LOAD)
        throw std::runtime_error("Failed to load skybox.");
#endif

    // load texture
    int w, h;
    uint8_t* data = stbi_load("../../../assets/textures/cobblestone.png", &w, &h, nullptr, 3);

    rt::ImageCreateInfo image_ci = {};
    image_ci.width = w;
    image_ci.height = h;
    image_ci.depth = 1;
    image_ci.channels = 3;

    this->tex.set_address_mode(rt::RT_TEXTURE_ADDRESS_MODE_REPEAT, rt::RT_TEXTURE_ADDRESS_MODE_REPEAT, rt::RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_BORDER);
    this->tex.set_filter(rt::RT_FILTER_NEAREST);
    this->tex.set_border_color(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    rt::ImageError error = this->tex.load(image_ci, data);
    if(error != rt::RT_IMAGE_ERROR_NONE)
        throw std::runtime_error("Failed to load texture.");
    std::cout << "texture loaded" << std::endl;
    stbi_image_free(data);

    // load spherical map
    this->spherical_env.set_address_mode(rt::RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_BORDER, rt::RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_BORDER, rt::RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_BORDER);
    this->spherical_env.set_filter(rt::RT_FILTER_LINEAR);
    error = rt::TextureLoader::loadf(this->spherical_env, "../../../assets/skyboxes/environment.hdr", 3);
    if (error != rt::RT_IMAGE_ERROR_NONE)
        throw std::runtime_error("Failed to load spherical map.");
    std::cout << "spherical map loaded" << std::endl;

    // load cubemap
    rt::CubemapCreateInfo cci = {};
    cci.right = "../../../assets/skyboxes/right.jpg";
    cci.left = "../../../assets/skyboxes/left.jpg";
    cci.top = "../../../assets/skyboxes/top.jpg";
    cci.bottom = "../../../assets/skyboxes/bottom.jpg";
    cci.front  = "../../../assets/skyboxes/front.jpg";
    cci.back = "../../../assets/skyboxes/back.jpg";

    error = rt::TextureLoader::load_cube(this->cubemap, cci, 3);
    if (error != rt::RT_IMAGE_ERROR_NONE)
        throw std::runtime_error("Failed to load cubemap.");
    std::cout << "cubemap loaded" << std::endl;

    rt::ImageCreateInfo fbo_ci = {};
    fbo_ci.width = SCR_WIDTH;
    fbo_ci.height = SCR_HEIGHT;

    rt::BufferLayout buffer_layout;
    buffer_layout.size = PRIM_COUNT;
    buffer_layout.first = 0;
    buffer_layout.last = buffer_layout.size;

    rt::Buffer buff(buffer_layout);
    buff.data(0, 3, spheres);

    this->set_num_threads(1);
    this->set_framebuffer(fbo_ci);
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
        const size_t size = this->rt_geometry()[b].layout().size;

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

float RT_Application::shadow(const rt::ray_t& shadow_ray, float t_max, float softness)
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

glm::vec3 RT_Application::ray_generation_shader(uint32_t x, uint32_t y)
{
    float ndc_x = gl::convert::from_pixels_pos_x(x, this->rt_dimensions().x) * this->rt_ratio();
    float ndc_y = gl::convert::from_pixels_pos_y(y, this->rt_dimensions().y);

    glm::vec3 origin    = {0.0f, -0.99f, 0.0f};                                              // origin of the camera
    glm::vec3 look_at   = {0.0f, -1.0f, 1.0f};                                               // direction/point the camere is looking at

    glm::vec3 cam_z     = glm::normalize(look_at - origin);                                 // z-direction of the camera
    glm::vec3 cam_x     = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), cam_z));   // x-diretion of the camera
    glm::vec3 cam_y     = glm::cross(cam_z, cam_x);                                         // y-direction of the camera

    // generate ray
    rt::ray_t ray;
    ray.origin = origin;
    ray.direction = glm::normalize(ndc_x * cam_x + ndc_y * cam_y + 1.5f * cam_z);                   // rotated intersection with the image plane

    // render the image in hdr
    glm::vec3 hdr_color(0.0f);
    trace_ray(ray, RT_RECURSIONS, 100.0f, rt::RT_CULL_MASK_NONE, &hdr_color);                                                  // begin ray-tracing process                              
    glm::vec3 ldr_color = hdr_color / (hdr_color + glm::vec3(1.0f));                        // convert to ldr

    return ldr_color;
}

void RT_Application::closest_hit_shader(const rt::ray_t& ray, int recursion, float t, float t_max, const rt::Primitive* hit, rt::RayHitInformation hit_info, void* ray_payload)
{
    rt::Sphere* hit_sphere = (rt::Sphere*)hit;
    glm::vec3* out_color = (glm::vec3*)ray_payload;

    glm::vec3 intersection = ray.origin + (t) * ray.direction;  // prevent self-intersection
    const glm::vec3 normal = glm::normalize(intersection - hit_sphere->center());
    const glm::vec3 view = -ray.direction;

    glm::vec3 color(0.0f);

    glm::vec3 refract_normal;
    float n;
    rt::RayCullMask cull_mask = rt::RT_CULL_MASK_NONE;
    if (hit_info & rt::RT_HIT_INFO_FRONT_BIT)
    {
        refract_normal = normal;
        n = 1.0f / 1.5f; 
    }
    else if (hit_info & rt::RT_HIT_INFO_BACK_BIT)
    {
        refract_normal = -normal;
        n = 1.5f;
    }

    rt::ray_t _sample_ray;
    _sample_ray.direction = glm::reflect(ray.direction, normal);
    //_sample_ray.direction = glm::refract(ray.direction, refract_normal, n);
    _sample_ray.origin = offset_ray_origin(intersection, normal, _sample_ray.direction);
    this->trace_ray(_sample_ray, recursion-1, t_max, cull_mask, &color);
    *out_color = color;
}

void RT_Application::miss_shader(const rt::ray_t& ray, int recursuon, float t_max, void* ray_payload)
{
    //glm::vec3 color = glm::vec3(this->cubemap.sample(ray.direction));
    //constexpr float HDRmax = 2.0f;
    //const float x = HDRmax / (HDRmax + 1);
    //color = (color * x) / (glm::vec3(1.0f) - color * x);
    //*((glm::vec3*)ray_payload) = color;

    *((glm::vec3*)ray_payload) = this->spherical_env.sample(glm::vec4(ray.direction.x, ray.direction.y, ray.direction.z, 0.0f));
}

void RT_Application::app_run(void)
{
    this->run();
}

const uint8_t* RT_Application::fetch_pixels(void) noexcept
{
    return this->get_framebuffer().map_rdonly();
}