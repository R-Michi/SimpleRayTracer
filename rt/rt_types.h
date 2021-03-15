#ifndef __rt_types__
#define __rt_types__

#include <glm/glm.hpp>

#define RT_INTERSECTION_CONSIDER_INSIDE 0x0001

// RT_BUFFER_... flags are counting from 0x0100 upwards
#define RT_BUFFER_FIRST                 0x0100  // indicates the begin of the RT_BUFFER_... flags
#define RT_BUFFER_NONE                  0x0100  // the buffer is undefined, this is a default value
#define RT_BUFFER_INDEX_COUNT_FORWARD   0x0101  // automatic indexing, counting forward
#define RT_BUFFER_INDEX_COUNT_BACKWARD  0x0102  // automatic indexing, counting backward
#define RT_BUFFER_INDEX_STATIC          0x0103  // user defined indices, static memory
#define RT_BUFFER_INDEX_DYNAMIC         0x0104  // user defined indices, dynamic memory, memory is cleared by the application
#define RT_BUFFER_LAST                  0x0104  // indicates the end of the RT_BUFFER_... flags

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

    struct BufferLayoutInfo
    {
        uint16_t info = RT_BUFFER_NONE;     // defines how the buffer should behave
        size_t size = 0;                    // the number of primitives the buffer can store
        size_t first = 0;                   // first primitive that is processed
        size_t last = 0;                    // last primitive that is processed
        int* indices = nullptr;             // user defined indices, only works if buffer should not count indices automatically
    };
}

#endif // __rt_types__