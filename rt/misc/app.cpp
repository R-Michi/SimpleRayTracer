/**
* @file     app.cpp
* @brief    Implementation of the ray-tracing pipeline and shader stages.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#include "app.h"
#include <omp.h>

#define atXY(x, y, stride) (y * stride + x)

using namespace rt;

RayTracer::RayTracer(void) noexcept
{
    this->_rt_dimensions = {0, 0};
    this->_rt_ratio = 0.0f;
    this->_rt_pixels = 0;
    this->_n_threads = 1;
}

RayTracer::~RayTracer(void) noexcept
{
}

float RayTracer::intersection(const rt::ray_t& ray, float t_max, int flags, const rt::Primitive** hit_prim)
{
    float t = t_max;
    const size_t bs = this->rt_geometry_buffer_count();

    // for each buffer...
    for(size_t b = 0; b < bs; b++)
    {
        // and for each primitive...
        const Primitive * const * map = this->rt_geometry()[b].map_rdonly();
        const size_t first = this->rt_geometry()[b].layout().first;
        const size_t last = this->rt_geometry()[b].layout().last;

        for(size_t p = first; p < last; p++)
        {
            // if it is not a nullptr...
            if(map[p] != nullptr)
            {
                // test for intersection...
                const float t_cur = map[p]->intersect(ray, t_max, flags);
                // and return the closest hit.
                if(t_cur < t)
                {
                    if(hit_prim != nullptr)
                        *hit_prim = map[p];
                    t = t_cur;
                }
            }
        }
    }
    return t;
}

void RayTracer::trace_ray(const ray_t& ray, int recursions, float t_max, void* ray_payload)
{
    if (recursions == 0) return;

    const Primitive* hit_prim = nullptr;
    float t = this->intersection(ray, t_max, 0, &hit_prim);

    if(t < t_max)   this->closest_hit_shader(ray, recursions, t, t_max, hit_prim, ray_payload);
    else            this->miss_shader(ray, recursions, t_max, ray_payload);
}

void RayTracer::run(void)
{
    omp_set_num_threads(this->_n_threads);
    uint8_t* map = this->_fbo.map_rdwr();

    #pragma omp parallel for
    for (uint32_t y = 0; y < this->_fbo.height(); y++)
    {
        for (uint32_t x = 0; x < this->_fbo.width(); x++)
        {
            const size_t idx = this->_fbo.combute_index({ x, y });
            this->cvt_to_uint8(ray_generation_shader(x, y), map[idx + 0], map[idx + 1], map[idx + 2]);
        }
    }
}

void RayTracer::set_framebuffer(const ImageCreateInfo& ci) noexcept
{
    this->_rt_dimensions    = {ci.width, ci.height};
    this->_rt_ratio         = (float)ci.width / (float)ci.height;
    this->_rt_pixels        = ci.width * ci.height;

    // reallocate image memory if create info changes
    this->_fbo.free();
    this->_fbo.set_create_info({ci.width, ci.height, 0, 3}); // framebuffer contains only RGB (3 color channels)
    this->_fbo.create();
}

const Framebuffer& RayTracer::get_framebuffer(void) const noexcept
{
    return this->_fbo;
}

void RayTracer::clear_color(float r, float g, float b)
{
    uint8_t* map = this->_fbo.map_rdwr();
    for (uint32_t y = 0; y < this->_fbo.height(); y++)
    {
        for (uint32_t x = 0; x < this->_fbo.width(); x++)
        {
            const size_t idx = this->_fbo.combute_index({ x, y });
            uint8_t _r, _g, _b;
            this->cvt_to_uint8({ r, g, b }, map[idx + 0], map[idx + 1], map[idx + 2]);
        }
    }
}

void RayTracer::draw_buffer(const Buffer& buff)
{
    this->_cmd_buff.push_back(buff);
}

void RayTracer::set_num_threads(uint32_t n_threads) noexcept
{
    this->_n_threads = (n_threads > 0) ? n_threads : this->_n_threads;
}