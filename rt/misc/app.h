/**
* @file     app.h
* @brief    Ray-tracing pipeline and shader stages.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "buffer.h"
#include "../image/framebuffer.h"
#include <vector>

namespace rt
{
    /**
     *  This class provides the interface to write a ray tracing application.
     *  The ray tracer renders the scene CPU-side and is not suited for real-time
     *  applications, instead its purpose is to render statis images with ray tracing.
     *  The ray tracer is designed to make context of only one thread. If you want 
     *  to use more threads to update properties of the ray tracing application, YOU have
     *  to synchronize the threads by YOURSELF.
     */
    class RayTracer
    {
    private:

        glm::i32vec2 _rt_dimensions;    // screen dimensions
        float _rt_ratio;                // screen aspect ratio
        int32_t _rt_pixels;             // number of pixels the framebuffer has
        std::vector<Buffer> _cmd_buff;  // command buffer for drawing
        Framebuffer _fbo;               // framebuffer where the pixels get stored
        uint32_t _n_threads;            // number of threads used for rendering

        /**
         *  @brief Tests if a ray intersects with a primitive in the scene..
         *  @param[in] ray: The ray that is tested if it intersects with a primitive.
         *  @param[in] t_max: The maximum length of the ray. Similar to the render distance.
         *  @param[in] flags: Can change the way how intersection is calculated.
         *  @param[out] hit_prim: The primitive that the ray intersected with.
         *  @return The length of the ray-origin to the closest intersection point.
         */
        float intersection(const rt::ray_t& ray, float t_max, int flags, const rt::Primitive** hit_prim);

        /**
        *   @brief Converts floating-point color to uint8_t-color value.
        *   @param[in] v: floating-point color vector
        *   @param[out] r: red in uint8_t
        *   @param[out] g: green in uint8_t
        *   @param[out] b: blue in uint8_t
        */
        inline void cvt_to_uint8(const glm::vec3& v, uint8_t& r, uint8_t& g, uint8_t& b)
        {
            glm::vec3 _v = v * 255.0f;
            r = (_v.r > 255.0) ? 255 : (uint8_t)_v.r;
            g = (_v.g > 255.0) ? 255 : (uint8_t)_v.g;
            b = (_v.b > 255.0) ? 255 : (uint8_t)_v.b;
        }

    protected:
        /** @return The pixel-dimensions of the framebuffer. */
        inline const glm::i32vec2& rt_dimensions(void) noexcept
        {return this->_rt_dimensions;}

        /** @return The screen aspect ratio. */
        inline float rt_ratio(void) noexcept
        {return this->_rt_ratio;}

        /** @return The numbers of pixels to process. */
        inline int32_t rt_pixels(void) noexcept
        {return this->_rt_pixels;}

        /** @return The whole scene-geometry. Can be multiple primitive-buffers. */
        inline const Buffer* rt_geometry(void) noexcept
        {return this->_cmd_buff.data();}

        /** @return The number of primitive buffers. */
        inline size_t rt_geometry_buffer_count(void) noexcept
        {return this->_cmd_buff.size();}

        /**
         *  @brief Begins the ray-tracing process.
         *  @param[in] ray: Ray to be traced.
         *  @param[in] recursions: The number of recursions to be called.
         *  @param[in] t_max: Maximum length the ray is allowed to have.
         *  @param[out] ray_payload: Ray tracing payload.
         */
        void trace_ray(const ray_t& ray, int recursions, float t_max, void* ray_payload);

        /**
         *  @brief This shader gets called for every pixel and is used calculate
         *  the final color of the current pixel. It is also used to invoke 
         *  the ray tracing process, why it is called ray generation shader.
         *  @param[in] x: X coordinate of the normalized screen space.
         *  @param[in] y: Y coordinate of the normalized screen space.
         *  @return Color of the current pixel.
         */
        virtual glm::vec3 ray_generation_shader(uint32_t x, uint32_t y) = 0;

        /**
         *  @brief This shader gets called if there is an intersection with a sphere.
         *  @param[in] ray: The ray that was traced to that intersction.
         *  @param[in] recursion: The actual recursion of the ray-tracing process.
         *  @param[in] t: The length of the vector from origin to the intersection.
         *  @param[in] t_max: The maximum length of any ray.
         *  @param[in] hit_prim: The primitive that the ray intersected with.
         *  @param[out] ray_payload: Ray tracing payload.
         */
        virtual void closest_hit_shader(const ray_t& ray, int recursion, float t, float t_max, const Primitive* hit, void* ray_payload) = 0;

        /**
         *  @brief This shader gets called if there is no intersection with any object in the scene.
         *  @param[in] ray: The ray that was traced into the void.
         *  @param[in] recursion: The actual recursion of the ray-tracing process.
         *  @param[in] t_max: The maximum length of any ray.
         *  @param[out] ray_payload: Ray tracing payload.
         */
        virtual void miss_shader(const ray_t& ray, int recursuon, float t_max, void* ray_payload) = 0;
        
    public:
        RayTracer(void) noexcept;
        
        RayTracer(const RayTracer&) = delete;
        RayTracer& operator= (const RayTracer&) = delete;

        RayTracer(RayTracer&&) = delete;
        RayTracer& operator= (RayTracer&&) = delete;

        virtual ~RayTracer(void) noexcept;

        /**
         *  @brief Effectively runs the ray-tracing application.
         *  Processes the color for every pixel and stores the resulting color into
         *  the framebuffer. Additionally it provides the NDC coordinates for the ray generation shader.
         */
        void run(void);

        /**
         *  @brief Sets the create info for the internal frame-buffer.
         *  The image data will be written into the framebuffer object.
         *  NOTE: Frame-buffer gets reallocated if this method is called twice!
         *  @param[in] ci: The framebuffer to attach.
         */
        void set_framebuffer(const ImageCreateInfo& ci) noexcept;

        /**
         *  @return The attached framebuffer object of the ray tracer.
         *  Pixels can be fetched.
         */
        const Framebuffer& get_framebuffer(void) const noexcept;

        /**
         *  @brief Cleares the image with a RGB color.
         *  @param[in] buff: Pixel buffer
         *  @param[in] r: Red value
         *  @param[in] g: Green value
         *  @param[in] b: Blue value
         */
        void clear_color(float r, float g, float b);

        /**
         *  @brief Adds a draw buffer to the comand buffer.
         *  @param buff: Buffer to draw.
         */
        void draw_buffer(const Buffer& buff);

        /**
         *  @brief Sets the number of threads the ray tracer uses for rendering.
         *  @param n_threads: Number of threads.
         */
        void set_num_threads(uint32_t n_threads) noexcept;
        
        /** @return The number of threads used for rendering. */
        inline uint32_t get_num_threads(void) const noexcept
        {return this->_n_threads;}
    };
}