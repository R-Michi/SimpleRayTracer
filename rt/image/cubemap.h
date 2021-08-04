/**
* @file     cubemap.h
* @brief    Implementation of a cubemap texture.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "texture.h"
#include <map>

namespace rt
{
    template<typename T_src, typename T_dst>
    class Cubemap
    {
    private:
        Texture2D<T_src, T_dst> faces[6];

    public:
        /** @param[in] filter: filter operation  */
        explicit Cubemap(Filter filter = RT_FILTER_NEAREST)
        {
            for (uint32_t i = 0; i < 6; i++)
            {
                this->faces[i].set_filter(filter);
                this->faces[i].set_address_mode(RT_TEXTURE_ADDRESS_MODE_REPEAT, RT_TEXTURE_ADDRESS_MODE_REPEAT, RT_TEXTURE_ADDRESS_MODE_REPEAT);
            }
        }
        virtual ~Cubemap(void)
        {
            this->free();
        }

        /**
        *   @brief Loads one face into the cubemap.
        *   @param[in] ci: image create info
        *   @param[in] face: cubemap face
        *   @param[in] data: pixels
        *   @return image error
        */
        ImageError load(const ImageCreateInfo& ci, CubemapFace face, const T_src* data)
        {
            return this->faces[static_cast<uint32_t>(face)].load(ci, data);
        }

        /** @brief Frees the allocated memory of the cubemap. */
        void free(void)
        {
            for (uint32_t i = 0; i < 6; i++)
                this->faces[i].free();
        }

        /**
        *   @brief Samples the cubemap.
        *   @param[in] direction: sample direction
        *   @return Color of the sample.
        */
        glm::vec4 sample(const glm::vec3& direction)
        {
            /* GET UV COORDINATES */
            const glm::vec3 abs_dir = glm::abs(direction);
            bool is_x_positive = (direction.x >= 0.0f);
            bool is_y_positive = (direction.y >= 0.0f);
            bool is_z_positive = (direction.z >= 0.0f);

            float max_axis;
            glm::vec2 uv;
            uint32_t face_index = 0;

            // front face / positive X
            if (is_x_positive && abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z)
            {
                // u (0 to 1) goes from +z to -z
                // v (0 to 1) goes from -y to +y
                max_axis = abs_dir.x;
                uv.x = -direction.z;
                uv.y = direction.y;
                face_index = 0;
            }

            // back face / negative X
            if (!is_x_positive && abs_dir.x >= abs_dir.y && abs_dir.x >= abs_dir.z)
            {
                // u (0 to 1) goes from -z to +z
                // v (0 to 1) goes from -y to +y
                max_axis = abs_dir.x;
                uv.x = direction.z;
                uv.y = direction.y;
                face_index = 1;
            }

            // top face / postive Y
            if (is_y_positive && abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z)
            {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from +z to -z
                max_axis = abs_dir.y;
                uv.x = direction.x;
                uv.y = -direction.z;
                face_index = 2;
            }

            // bottom face / negative Y
            if (!is_y_positive && abs_dir.y >= abs_dir.x && abs_dir.y >= abs_dir.z)
            {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from -z to +z
                max_axis = abs_dir.y;
                uv.x = direction.x;
                uv.y = direction.z;
                face_index = 3;
            }

            // left face / postive Z
            if (is_z_positive && abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y)
            {
                // u (0 to 1) goes from -x to +x
                // v (0 to 1) goes from -y to +y
                max_axis = abs_dir.z;
                uv.x = direction.x;
                uv.y = direction.y;
                face_index = 4;
            }

            // right face / negative Z
            if (!is_z_positive && abs_dir.z >= abs_dir.x && abs_dir.z >= abs_dir.y)
            {
                // u (0 to 1) goes from +x to -x
                // v (0 to 1) goes from -y to +y
                max_axis = abs_dir.z;
                uv.x = -direction.x;
                uv.y = direction.y;
                face_index = 5;
            }

            // uv coordinates have a range of -1 to +1 -> convert to 0 to +1
            uv = 0.5f * (uv / max_axis + 1.0f);
            uv.y = 1.0f - uv.y;

            // sample face texture
            return this->faces[face_index].sample(glm::vec4(uv.x, uv.y, 0.0f, 0.0f));
        }
    };

    /**
    *   Load functions for typical image formats:
    *   1) 8-bit (per color channel) image format
    *   2) 16-bit (per color channel) image format
    *   3) floating-point image format
    */
    namespace TextureLoader
    {
        /**
        *   @brief Loads 6 8-bit (per color channel) images into a cubemap object.
        *   @param[out] tex: cubemap object
        *   @param[in] path: cubemap create info.
        *   @param[in] force_channels: Force the loader to load a certain number of color channels (1 - 4).
        *                              Leave it to 0 to not force the loader.
        *   @return image error
        */
        ImageError load_cube(Cubemap<uint8_t, float>& cubemap, const CubemapCreateInfo& cci, uint32_t force_channels);

        /**
        *   @brief Loads 6 16-bit (per color channel) images into a cubemap object.
        *   @param[out] tex: cubemap object
        *   @param[in] path: cubemap create info.
        *   @param[in] force_channels: Force the loader to load a certain number of color channels (1 - 4).
        *                              Leave it to 0 to not force the loader.
        *   @return image error
        */
        ImageError load_cube16(Cubemap<uint16_t, float>& cubemap, const CubemapCreateInfo& cci, uint32_t force_channels);

        /**
        *   @brief Loads 6 floating-point images into a cubemap object.
        *   @param[out] tex: cubemap object
        *   @param[in] path: cubemap create info.
        *   @param[in] force_channels: Force the loader to load a certain number of color channels (1 - 4).
        *                              Leave it to 0 to not force the loader.
        *   @return image error
        */
        ImageError load_cubef(Cubemap<float, float>& cubemap, const CubemapCreateInfo& cci, uint32_t force_channels);
    }
};