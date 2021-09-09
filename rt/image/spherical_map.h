/**
* @file     spherical_map.h
* @brief    Texture that samples a spherical environment from a 2D-image.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "texture.h"

namespace rt
{
    template<typename T_src, typename T_dst>
    class SphericalMap : public Texture2D<T_src, T_dst>
    {
    private:
        using vec_ret = typename SphericalMap::vec_ret;
    public:
        explicit SphericalMap(Filter filter = RT_FILTER_NEAREST, const vec_ret& border_color = vec_ret(0.0)) noexcept
        : Texture2D<T_src, T_dst>(filter, border_color) {}
        virtual ~SphericalMap(void) {}

        virtual vec_ret sample(const glm::vec4& direction) const
        {
            constexpr static glm::vec2 inv_atan(0.1591f, 0.3183);
            glm::vec2 uv(atan2(direction.x, direction.z), asin(direction.y));
            uv = uv * inv_atan + 0.5f;
            uv.y = 1.0f - uv.y;

            vec_ret color;
            this->_sample(glm::vec4(uv.x, uv.y, 0.0f, 0.0f), color);
            return color;
        }
    };
}