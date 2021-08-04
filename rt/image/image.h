/**
* @file     image.h
* @brief    Implementation of an image container.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../misc/rt_types.h"
#include "../misc/rt_error.h"
#include <array>
#include <malloc.h>
#include <iostream>

namespace rt
{
    template<typename T, uint32_t dimmensions>
    class Image
    {
        static_assert(dimmensions != 0, "Ray-Tracing: Image must not have 0 dimmensions.");
        static_assert(dimmensions <= 3, "Ray-Tracing: Image can only have a maximum of 3 dimmensions.");

    private:
        T* data;

    protected:
        alignas(16) ImageCreateInfo create_info; // needs 16-byte alignment for __m1288i to work as fast as possible

        // 1D image coordianate to array index
        static inline size_t image2array1D(uint32_t x)
        {
            return x;
        }

        // 2D image coordinate to array index
        static inline size_t image2array2D(uint32_t x, uint32_t y, const ImageCreateInfo& ci)
        {
            return (y * ci.width + x) * ci.channels;
        }

        // 3D image coordinate to array index
        static inline size_t image2array3D(uint32_t x, uint32_t y, uint32_t z, const ImageCreateInfo& ci)
        {
            return (z * ci.width * ci.height + y * ci.width + x) * ci.channels;
        }

    public:

        /**
        *   @brief Default constructor.
        */
        Image(void) noexcept
        {
            this->create_info = { 0,0,0,0 };
            this->data = nullptr;
        }

        /**
        *   @brief Constrcutor with create info as input
        *   @param[in] ci: ImageCreateInfo struct
        */
        explicit Image(const ImageCreateInfo& ci) noexcept
        {
            this->create_info = ci;
            this->data = nullptr;
        }

        /**
        *   @brief Destructor.
        */
        virtual ~Image(void)
        {
            this->free();
        }

        /**
        *   @brief Sets the create info for the image.
        *   @param[in] ci: ImageCreateInfo struct
        */
        void set_create_info(const ImageCreateInfo& ci) noexcept
        {
            this->create_info = ci;
        }

        /**
        *   @brief Allocates memory for the image.
        *   @return Image error (RT_IMAGE_ERROR_OUT_OF_MEMORY, RT_IMAGE_ERROR_NONE).
        */
        ImageError create(void) noexcept
        {
            if (this->data == nullptr)
            {
                switch (dimmensions)
                {
                case 1:
                    this->data = (T*)malloc(sizeof(T) * this->create_info.channels * this->create_info.width);
                    break;
                case 2:
                    this->data = (T*)malloc(sizeof(T) * this->create_info.channels * this->create_info.width * this->create_info.height);
                    break;
                case 3:
                    this->data = (T*)malloc(sizeof(T) * this->create_info.channels * this->create_info.width * this->create_info.height * this->create_info.depth);
                    break;
                }
                if (this->data == nullptr)
                    return RT_IMAGE_ERROR_OUT_OF_MEMORY;
            }
            return RT_IMAGE_ERROR_NONE;
        }

        /**
        *   @brief Frees the allocated memory of the image.
        */
        void free(void) noexcept
        {
            if (this->data != nullptr)
            {
                std::free(this->data);
                this->data = nullptr;
            }
        }

        /**
        *   @brief Writes a single pixel to the image.
        *   @param[in] pos: XYZ-Position of the pixel.
        *   @param[in] data: Data that should be written to the image.
        *   @return Image error (RT_IMAGE_ERROR_NULL, RT_IMAGE_ERROR_OUT_OF_RANGE, RT_IMAGE_ERROR_NONE)
        */
        ImageError write_pixel(const glm::vec<dimmensions, uint32_t, glm::defaultp>& pos, const T* data) noexcept
        {
            if (data == nullptr)
                return RT_IMAGE_ERROR_NULL;

            size_t idx = this->combute_index(pos);
            if (idx >= this->max_index())
                return RT_IMAGE_ERROR_OUT_OF_RANGE;

            memcpy(this->data + idx, data, sizeof(T) * this->create_info.channels);
            return RT_IMAGE_ERROR_NONE;
        }

        /**
        *   @brief Reads a single pixel from the image.
        *   @param[in] pos: XYZ-Position of the pixel.
        *   @param[out] data: Pointer where the pixel data should be stored.
        *   @return Image error (RT_IMAGE_ERROR_NULL, RT_IMAGE_ERROR_OUT_OF_RANGE, RT_IMAGE_ERROR_NONE)
        */
        void read_pixel(const glm::vec<dimmensions, uint32_t, glm::defaultp>& pos, T* data) const noexcept
        {
            if (data == nullptr)
                return RT_IMAGE_ERROR_NULL;

            size_t idx = this->combute_index(pos);
            if (idx >= this->max_index(pos))
                return RT_IMAGE_ERROR_OUT_OF_RANGE;

            memcpy(data, this->data + this->combute_index(pos), sizeof(T) * this->create_info.channels);
            return RT_IMAGE_ERROR_NONE;
        }

        /**
        *   @brief Converts a XYZ-position to an array index.
        *   @param img_pos: Image XYZ-position.
        *   @return Array index.
        */
        size_t combute_index(const glm::vec<dimmensions, uint32_t, glm::defaultp>& img_pos) const noexcept
        {
            uint32_t* p = (uint32_t*)&img_pos;
            switch (dimmensions)
            {
            case 1: return image2array1D(p[0]);
            case 2: return image2array2D(p[0], p[1], this->create_info);
            case 3: return image2array3D(p[0], p[1], p[2], this->create_info);
            }
            return 0;
        }

        /**
        *   @return A read-write-access pointer to the image data.
        */
        inline T* map_rdwr(void) noexcept
        {
            return this->data;
        }

        /**
        *   @return A read-only-access pointer to the image data.
        */
        inline const T* map_rdonly(void) const noexcept
        {
            return this->data;
        }

        /**
        *   @return The number of pixel the image can store.
        */
        inline size_t count(void)
        {
            switch (dimmensions)
            {
            case 1: return this->create_info.width;
            case 2: return this->create_info.width * this->create_info.height;
            case 3: return this->create_info.width * this->create_info.height * this->create_info.depth;
            }
            return 0;
        }

        /**
        *   @return The size in bytes of the image.
        */
        inline size_t size(void)
        {
            return sizeof(T) * this->create_info.channels * this->count();
        }

        /**
        *   @return Width of the image.
        */
        inline uint32_t width(void)
        {
            return this->create_info.width;
        }

        /**
        *   @return Height of the image.
        */
        inline uint32_t height(void)
        {
            return (dimmensions > 1) ? this->create_info.height : 1;
        }

        /**
        *   @return Depth of the image.
        */
        inline uint32_t depth(void)
        {
            return (dimmensions > 2) ? this->create_info.depth : 1;
        }

        /**
        *   @return Number of color channels of the image.
        */
        inline uint32_t channel_count(void)
        {
            return this->create_info.channels;
        }
    };

    template<typename T> using Image1D = Image<T, 1>;
    template<typename T> using Image2D = Image<T, 2>;
    template<typename T> using Image3D = Image<T, 3>;
}