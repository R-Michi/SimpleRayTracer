#ifndef __rt_types__
#define __rt_types__

#include <glm/glm.hpp>

#define RT_INTERSECTION_CONSIDER_INSIDE 0x0001

namespace rt
{
    struct Ray
    {
        glm::vec3 origin;
        glm::vec3 direction;
    };

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

    struct Light
    {
        glm::vec3 direction;
        glm::vec3 intensity;
    };

    struct Material
    {
        glm::vec3 albedo;
        float roughness;
        float metallic;
        float apha;
    };

    struct BufferLayout
    {
        size_t size = 0;                    // the number of primitives the buffer can store
        size_t first = 0;                   // first primitive that is processed
        size_t last = 0;                    // last primitive that is processed
    };

    struct ImageLayout
    {
        uint32_t width;
        uint32_t height;
        uint32_t channels;
    };

    struct Image
    {
        ImageLayout layout;
        uint8_t*    data;
    };

    enum Filter
    {
        RT_FILTER_NEAREST = 0x0,
        RT_FILTER_LINEAR = 0x1
    };

    enum CubemapFace
    {
        RT_CUBEMAP_FACE_FRONT = 0x0,
        RT_CUBEMAP_FACE_BACK = 0x1,
        RT_CUBEMAP_FACE_TOP = 0x2,
        RT_CUBEMAP_FACE_BOTTOM = 0x3,
        RT_CUBEMAP_FACE_LEFT = 0x4,
        RT_CUBEMAP_FACE_RIGHT = 0x5,
    };

    struct CubemapCreateInfo
    {
        const char* front;
        const char* back;
        const char* top;
        const char* bottom;
        const char* left;
        const char* right;
        Filter      filter;
    };

    struct Framebuffer
    {
        int32_t width    = 0;
        int32_t height   = 0;
        Color3ui8* buff = nullptr;
    };

}

#endif // __rt_types__