#ifndef __rt_application_h__
#define __rt_application_h__

#include "rt_types.h"
#include "primitive.h"
#include "buffer.h"

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

        glm::i32vec2 _rt_frag_coord;    // pixel coordinates of the current pixel.
        glm::i32vec2 _rt_dimensions;    // screen dimensions
        float _rt_ratio;                // screen aspect ratio
        int32_t _rt_pixels;             // number of pixels the framebuffer has
        std::vector<Buffer> _cmd_buff;  // command buffer for drawing
        Framebuffer _fbo;               // framebuffer where the pixels get stored
        uint32_t _n_threads;            // number of threads used for rendering

        /**
         *  Tests if a ray intersects with a primitive in the scene..
         *  @param ray -> The ray that is tested if it intersects with a primitive.
         *  @param t_max -> The maximum length of the ray. Similar to the render distance.
         *  @param flags -> Can change the way how intersection is calculated.
         *  @return @param hit_prim -> The primitive that the ray intersected with.
         *  @return -> The length of the ray-origin to the closest intersection point.
         */
        float intersection(const rt::Ray& ray, float t_max, int flags, const rt::Primitive** hit_prim);

    protected:
        // global read only values
        /// Fragment coordinate @return the pixel coordinate of the current processed pixel.
        inline const glm::i32vec2& rt_frag_coord(void) noexcept
        {return this->_rt_frag_coord;}

        /// @return The pixel-dimensions of the framebuffer.
        inline const glm::i32vec2& rt_dimensions(void) noexcept
        {return this->_rt_dimensions;}

        /// @return The screen aspect ratio.
        inline float rt_ratio(void) noexcept
        {return this->_rt_ratio;}

        /// @return The numbers of pixels to process.
        inline int32_t rt_pixels(void) noexcept
        {return this->_rt_pixels;}

        /// @return The while scene-geometry. Can be multiple primitive-buffers.
        inline const Buffer* rt_geometry(void) noexcept
        {return this->_cmd_buff.data();}

        /// @return The number of primitive buffers.
        inline size_t rt_geometry_num_buffers(void) noexcept
        {return this->_cmd_buff.size();}

        /**
         *  Begins the ray-tracing process.
         *  @param ray -> Ray to be traced.
         *  @param recursions -> The number of recursions to be called.
         *  @param t_max -> Maximum length the ray is allowed to have.
         *  @return -> The light intensity (color) of the intersecion.
         */
        glm::vec3 trace_ray(const Ray& ray, int recursions, float t_max);

        /**
         *  This shader gets called for every pixel and is used calculate
         *  the final color of the current pixel.
         *  @param x -> X coordinate of the normalized screen space.
         *  @param y -> Y coordinate of the normalized screen space.
         *  @return -> Color of the current pixel.
         */
        virtual glm::vec3 main_shader(float x, float y) = 0;

        /**
         *  This shader gets called if there is an intersection with a sphere.
         *  @param ray -> The ray that was traced to that intersction.
         *  @param recursion -> The actual recursion of the ray-tracing process.
         *  @param t -> The length of the vector from origin to the intersection.
         *  @param t_max -> The maximum length of any ray.
         *  @param hit_prim -> The primitive that the ray intersected with.
         *  @return -> A color-value of the current intersection point.
         */
        virtual glm::vec3 closest_hit_shader(const Ray& ray, int recursion, float t, float t_max, const Primitive* hit) = 0;

        /**
         *  This shader gets called if there is no intersection with any object in the scene.
         *  @param ray -> The ray that was traced into the void.
         *  @param recursion -> The actual recursion of the ray-tracing process.
         *  @param t_max -> The maximum length of any ray.
         *  @return -> A color-value if the ray misses.
         */
        virtual glm::vec3 miss_shader(const Ray& ray, int recursuon, float t_max) = 0;
        
    public:
        RayTracer(void) noexcept;
        
        RayTracer(const RayTracer&) = delete;
        RayTracer& operator= (const RayTracer&) = delete;

        RayTracer(RayTracer&&) = delete;
        RayTracer& operator= (RayTracer&&) = delete;

        virtual ~RayTracer(void) noexcept;

        /**
         *  Effectively runs the ray-tracing application.
         *  Processes the color for every pixel and stores the resulting color into
         *  the framebuffer.
         *  Additionally it provides the NDC coordinates for the ray generation shader.
         */
        void run(void);

        /**
         *  Attaches the framebuffer object to the ray tracer.
         *  The image data will be written into the framebuffer object.
         *  @param fbo -> The framebuffer to attach.
         */
        void set_framebuffer(const Framebuffer& fbo) noexcept;

        /**
         *  @return -> The attached framebuffer object of the ray tracer.
         *             Pixels can be fetched.
         */
        const Framebuffer& get_framebuffer(void) const noexcept;

        /**
         *  Cleares the image with a RGB color.
         *  @param buff -> Pixel buffer
         *  @param r -> Red value
         *  @param g -> Green value
         *  @param b -> Blue value
         */
        void clear_color(float r, float g, float b);

        /**
         *  Adds a draw buffer to the comand buffer.
         *  @param buff -> Buffer to draw.
         */
        void draw_buffer(const Buffer& buff);

        /**
         *  Sets the number of threads the ray tracer uses for rendering.
         *  @param n_threads -> Number of threads.
         */
        void set_num_threads(uint32_t n_threads) noexcept;
        
        /// @return The number of threads used for rendering.
        inline uint32_t get_num_threads(void) const noexcept
        {return this->_n_threads;}
    };
}

#endif //__rt_application_h__