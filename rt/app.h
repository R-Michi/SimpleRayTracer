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
        // global values
        glm::vec2 _rt_frag_coord;       // pixel coordinates of the current pixel.
        uint32_t _rt_primitive_id;      // id of the current processes primitive.
        uint32_t _rt_buffer_index;      // idnex of the buffer that is drawn. (index in the command buffer)

        std::vector<Buffer> cmd_buff;   // command buffer for drawing
        float _t_max;                   // maximum length of the ray

        /**
         *  Effectively runs the ray-tracing application.
         *  Processes the color for every pixel and stores the resulting color into
         *  the framebuffer.
         *  Additionally it provides the NDC coordinates for the ray generation shader.
         */
        void run(void);


    protected:
        /// @return -> The pixel coordinates of the current pixel.
        inline const glm::vec2& rt_frag_coord(void) noexcept
        {return this->_rt_frag_coord;}

        /// @return -> The id if the current processed primitive.
        inline uint32_t rt_primitive_id(void) noexcept
        {return this->_rt_primitive_id;} 

        inline uint32_t rt_buffer_index(void) noexcept
        {return this->_rt_buffer_index;}

        /**
         *  Begins the ray-tracing process.
         *  @param ray -> Ray to be traced.
         *  @param recursions -> The number of recursions to be called.
         *  @return -> The light intensity (color) of the intersecion.
         */
        glm::vec3 trace_ray(const Ray& ray, int recursions);

        /**
         *  This shader gets called for every pixel and is used to generate
         *  the main ray from the camera into the scene.
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
        RayTracer(void);
        virtual ~RayTracer(void);

        /**
         *  Attaches the framebuffer object to the ray tracer.
         *  The image data will be written into the framebuffer object.
         *  @param fbo -> The framebuffer to attach.
         */
        void set_framebuffer(const Framebuffer& fbo);

        /**
         *  @return -> The attached framebuffer object of the ray tracer.
         *             Pixels can be fetched.
         */
        const Framebuffer& get_framebuffer(void) const noexcept;

        /**
         *  Sets the maximum length of the ray.
         *  @param _t_max -> Maximum ray length.
         */
        void set_ray_maximum_length(float _t_max) noexcept;

        /**
         *  Cleares the image with a RGB color.
         *  @param buff -> Pixel buffer
         *  @param r -> Red value
         *  @param g -> Green value
         *  @param b -> Blue value
         */
        void clear_color(float r, float g, float b);

        /**
         *  Adds a command to the command buffer.
         *  @param buff -> Buffer to draw.
         */
        void draw_buffer(const Buffer& buff);
    };
}

#endif //__rt_application_h__