#ifndef __rt_types__
#define __rt_types__

#include <glm/glm.hpp>

#define RT_INTERSECTION_CONSIDER_INSIDE 0x1

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
};

#endif // __rt_types__