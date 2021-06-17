#include "../app.h"
#include "../glc-1.0.0/GL/glc.h"
#include <omp.h>

#define atXY(x, y, stride) (y * stride + x)

using namespace rt;

RayTracer::RayTracer(void) noexcept
{
    this->_rt_dimensions = {0, 0};
    this->_rt_ratio = 0.0f;
    this->_rt_pixels = 0;
    this->_fbo = {0, 0, nullptr};
    this->_n_threads = 1;
}

RayTracer::~RayTracer(void) noexcept
{
    // deallocate memory when ray tracer gets destroyed
    if(this->_fbo.buff != nullptr)
    {
        free(this->_fbo.buff);
        this->_fbo.buff = nullptr;
    }
}

float RayTracer::intersection(const rt::Ray& ray, float t_max, int flags, const rt::Primitive** hit_prim)
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

void RayTracer::trace_ray(const Ray& ray, int recursions, float t_max, void* ray_payload)
{
    if (recursions == 0) return;

    const Primitive* hit_prim = nullptr;
    float t = this->intersection(ray, t_max, 0, &hit_prim);

    if(t < t_max)
        this->closest_hit_shader(ray, recursions, t, t_max, hit_prim, ray_payload);
    else
        this->miss_shader(ray, recursions, t_max, ray_payload);
}

void RayTracer::run(void)
{
    if(this->_fbo.buff != nullptr)
    {
        omp_set_num_threads(this->_n_threads);
        #pragma omp parallel for
        for(int32_t y = 0; y < this->_fbo.height; y++)
        {
            for(int32_t x = 0; x < this->_fbo.width; x++)
            {
                const float ndc_x = gl::convert::from_pixels_pos_x(x, this->_fbo.width);
                const float ndc_y = gl::convert::from_pixels_pos_y(y, this->_fbo.height);
                this->_fbo.buff[atXY(x, y, this->_fbo.width)] = ray_generation_shader(ndc_x, ndc_y);
            }
        }
    }
}

void RayTracer::set_framebuffer(const Framebuffer& fbo) noexcept
{
    this->_fbo.width    = fbo.width;
    this->_fbo.height   = fbo.height;
    
    // clear memory if framebuffer changes
    if(this->_fbo.buff != nullptr)
        free(this->_fbo.buff);
    this->_fbo.buff = (Color3ui8*)malloc(sizeof(Color3ui8) * this->_fbo.width * this->_fbo.height);

    this->_rt_dimensions    = {this->_fbo.width, this->_fbo.height};
    this->_rt_ratio         = (float)this->_fbo.width / (float)this->_fbo.height;
    this->_rt_pixels        = this->_fbo.width * this->_fbo.height;
}

const Framebuffer& RayTracer::get_framebuffer(void) const noexcept
{
    return this->_fbo;
}

void RayTracer::clear_color(float r, float g, float b)
{
    if(this->_fbo.buff != nullptr)
    {
        for(int32_t i = 0; i < this->_rt_pixels; i++)
            this->_fbo.buff[i] = glm::vec3(r, g, b);
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