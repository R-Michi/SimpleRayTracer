#ifndef __rt_cubemap_h__
#define __rt_cubemap_h__

#ifdef __clang__	// suppress waring "empty body" for clang for include file stb_image.h
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wempty-body"
        #include <stb/stb_image.h>
    #pragma clang diagnostic pop
#else
    #include <stb/stb_image.h>
#endif

#include "rt_types.h"
#include "rt_error.h"
#include <map>

namespace rt
{
    class Cubemap
    {
    private:
        std::map<CubemapFace, Image> faces;
        Filter filter;

        TextureError load_face(CubemapFace face, const char* path, uint32_t channels);
        glm::vec2 get_cube_uv(const glm::vec3& dir, CubemapFace& face);
        void get_color(const Image& image, const glm::uvec2& px_coord, glm::vec4& color);

    public:
        Cubemap(void);
        Cubemap(const CubemapCreateInfo& info);
        ~Cubemap(void);

        TextureError load(const CubemapCreateInfo& info);
        void clear(void);
        glm::vec4 sample(const glm::vec3& direction);
    };
};

#endif // __rt_cubemap_h__