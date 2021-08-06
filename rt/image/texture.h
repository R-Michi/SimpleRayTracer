/**
* @file     texture.h
* @brief    Implementation of a texture, image sampling and filtering.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "image.h"
#include <type_traits>
#include <immintrin.h>

#ifdef __clang__	// suppress waring "empty body" for clang for include file stb_image.h
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wempty-body"
        #include <stb/stb_image.h>
    #pragma clang diagnostic pop
#else
    #include <stb/stb_image.h>
#endif

namespace rt
{
    template <typename T_src, typename T_dst, uint32_t dimmensions>
    class Texture : public Image<T_dst, dimmensions>
    {
        enum type_t
        {
            RT_INVALID_TYPE = 0,
            RT_INT_8 = 1,
            RT_UINT_8 = 2,
            RT_INT_16 = 3,
            RT_UINT_16 = 4,
            RT_INT_32 = 5,
            RT_UINT_32 = 6,
            RT_INT_64 = 7,
            RT_UINT_64 = 8,
            RT_FLOAT = 9,
            RT_DOUBLE = 10
        };

    protected:
        using vec_ret = glm::vec<4, T_dst, glm::defaultp>;

    private:
        static constexpr type_t get_src_type(void) noexcept
        {
            if (std::is_integral<T_src>::value && std::is_signed<T_src>::value      && sizeof(T_src) == sizeof(int8_t))     return RT_INT_8;
            if (std::is_integral<T_src>::value && std::is_unsigned<T_src>::value    && sizeof(T_src) == sizeof(uint8_t))    return RT_UINT_8;
            if (std::is_integral<T_src>::value && std::is_signed<T_src>::value      && sizeof(T_src) == sizeof(int16_t))    return RT_INT_16;
            if (std::is_integral<T_src>::value && std::is_unsigned<T_src>::value    && sizeof(T_src) == sizeof(uint16_t))   return RT_UINT_16;
            if (std::is_integral<T_src>::value && std::is_signed<T_src>::value      && sizeof(T_src) == sizeof(int32_t))    return RT_INT_32;
            if (std::is_integral<T_src>::value && std::is_unsigned<T_src>::value    && sizeof(T_src) == sizeof(uint32_t))   return RT_UINT_32;
            if (std::is_integral<T_src>::value && std::is_signed<T_src>::value      && sizeof(T_src) == sizeof(int64_t))    return RT_INT_64;
            if (std::is_integral<T_src>::value && std::is_unsigned<T_src>::value    && sizeof(T_src) == sizeof(uint64_t))   return RT_UINT_64;
            if (std::is_floating_point<T_src>::value && sizeof(T_src) == sizeof(float))  return RT_FLOAT;
            if (std::is_floating_point<T_src>::value && sizeof(T_src) == sizeof(double)) return RT_DOUBLE;
            return RT_INVALID_TYPE;
        }
        static constexpr type_t get_dst_type(void) noexcept
        {
            if (std::is_integral<T_dst>::value && std::is_signed<T_dst>::value      && sizeof(T_dst) == sizeof(int8_t))     return RT_INT_8;
            if (std::is_integral<T_dst>::value && std::is_unsigned<T_dst>::value    && sizeof(T_dst) == sizeof(uint8_t))    return RT_UINT_8;
            if (std::is_integral<T_dst>::value && std::is_signed<T_dst>::value      && sizeof(T_dst) == sizeof(int16_t))    return RT_INT_16;
            if (std::is_integral<T_dst>::value && std::is_unsigned<T_dst>::value    && sizeof(T_dst) == sizeof(uint16_t))   return RT_UINT_16;
            if (std::is_integral<T_dst>::value && std::is_signed<T_dst>::value      && sizeof(T_dst) == sizeof(int32_t))    return RT_INT_32;
            if (std::is_integral<T_dst>::value && std::is_unsigned<T_dst>::value    && sizeof(T_dst) == sizeof(uint32_t))   return RT_UINT_32;
            if (std::is_integral<T_dst>::value && std::is_signed<T_dst>::value      && sizeof(T_dst) == sizeof(int64_t))    return RT_INT_64;
            if (std::is_integral<T_dst>::value && std::is_unsigned<T_dst>::value    && sizeof(T_dst) == sizeof(uint64_t))   return RT_UINT_64;
            if (std::is_floating_point<T_dst>::value && sizeof(T_dst) == sizeof(float)) return RT_FLOAT;
            if (std::is_floating_point<T_dst>::value && sizeof(T_dst) == sizeof(double)) return RT_DOUBLE;
            return RT_INVALID_TYPE;
        }

        static_assert(Texture::get_src_type() != RT_INVALID_TYPE,                                   
                        "[Ray-Tracer | Texture]: Source-type must be an integral or floating-point type.");
        static_assert(Texture::get_dst_type() == RT_FLOAT || Texture::get_dst_type() == RT_DOUBLE, 
                        "[Ray-Tracer | Texture]: Destination-type must be a floating-point type.");

        /**
        *   Converts source-type to destination-type.
        *   @param[in] v: source-type
        *   @return destination-type
        */
        static T_dst convert_type(T_src v) noexcept
        {
            // If src and dst are both floating-point type, the value only gets casted.
            if (std::is_floating_point<T_src>::value && std::is_floating_point<T_dst>::value) return static_cast<T_dst>(v);

            // If src is an integral type and dst as a floating-point type, the value gets normalized.
            // If dst-type is a float, the double value will be casted.
            return static_cast<T_dst>(v) / static_cast<T_dst>(std::numeric_limits<T_src>::max());
        }

        /**
        *   @brief Samples a single pixel from the texture without filter operations applied.
        *   @param[in] pos: pixel-coordinate
        *   @param[out] color: color of the pixel
        */
        void _sample_px(const glm::uvec4& pos, vec_ret& color) noexcept
        {
            glm::uvec4 _pos;
            this->combute_address_mode(pos, _pos);
            switch (dimmensions)
            {
            case 1: if (_pos.x >= this->create_info.width) { color = this->border_color; return; }
            case 2: if (_pos.x >= this->create_info.width || _pos.y >= this->create_info.height) { color = this->border_color; return; }
            case 3: if (_pos.x >= this->create_info.width || _pos.y >= this->create_info.height || _pos.z >= this->create_info.depth) { color = this->border_color; return; }
            }


            const T_dst* map = this->map_rdonly();      // read-only accesss to the image data array
            T_dst* const c = (T_dst*)&color;            // the same, but in pointer form, because the numer of channels is not known
            size_t idx = 0;                             // index to access the data array (base index of the pixel at the position @param[in] pos)

            switch (dimmensions)
            {
            case 1: idx = Texture::image2array1D(_pos.x); break;
            case 2: idx = Texture::image2array2D(_pos.x, _pos.y, this->create_info); break;
            case 3: idx = Texture::image2array3D(_pos.x, _pos.y, _pos.z, this->create_info); break;
            }

            // i is the channel index: access_index = pixel_base_index + channel_index
            for (uint32_t i = 0; i < 4; i++)
                c[i] = (i < this->create_info.channels) ? map[idx + i] : static_cast<T_dst>(0);
        }

        /**
        *   @brief Combutes the address-mode for one dimmension.
        *   @param[in] pos: x/y/z-pixel-coordinate
        *   @param[in] address_index: 0-indexed dimmension number
        *   @return x/y/z-pixel-coordinate after address mode computation.
        */
        uint32_t combute_address_mode_comp(uint32_t pos, size_t address_idx) const noexcept
        {
            const uint32_t* const cip = (uint32_t*)&this->create_info;
            const uint32_t s = cip[address_idx];

            if (this->address_mode[address_idx] == RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_BORDER) return (pos >= s) ? s : pos;
            if (this->address_mode[address_idx] == RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_EDGE)   return (pos >= s) ? s - 1 : pos;
            if (this->address_mode[address_idx] == RT_TEXTURE_ADDRESS_MODE_REPEAT)          return pos % s;
            if (this->address_mode[address_idx] == RT_TEXTURE_ADDRESS_MODE_MIRRORED_REPEAT) return (pos / s % 2 == 1) ? s - ((pos % s) + 1) : pos % s;
            return 0;
        }

    protected:
        Filter filter;
        TextureAddressMode address_mode[3];
        vec_ret border_color;

        /**
        *   @brief Samples the texture with filter operations applied.
        *   @praram[in] pos: uvw-coordinate
        *   @param[out] color: Color of the sample.
        */
        void _sample(const glm::vec4& pos, vec_ret& color) noexcept
        {
            alignas(16) glm::uvec4 px_pos;
            alignas(16) glm::vec4 interpos;
            alignas(16) glm::vec4 _pos = pos;
            this->combute_image_pos(&_pos, &px_pos, &interpos);

            if (this->filter == RT_FILTER_NEAREST)
            {
                _sample_px(px_pos, color);
                return; 
            }

            // linear filter
            if (dimmensions == 1)
            {
                glm::uvec4 pos0 = px_pos;
                glm::uvec4 pos1 = px_pos + glm::uvec4(1, 0, 0, 0);

                vec_ret c0, c1;
                _sample_px(pos0, c0);
                _sample_px(pos1, c1);
                color = glm::mix(c0, c1, interpos.x);
            }
            else if (dimmensions == 2)
            {
                glm::uvec4 pos00 = px_pos;
                glm::uvec4 pos10 = px_pos + glm::uvec4(1, 0, 0, 0);
                glm::uvec4 pos01 = px_pos + glm::uvec4(0, 1, 0, 0);
                glm::uvec4 pos11 = px_pos + glm::uvec4(1, 1, 0, 0);

                vec_ret c00, c10, c01, c11;
                _sample_px(pos00, c00);
                _sample_px(pos10, c10);
                _sample_px(pos01, c01);
                _sample_px(pos11, c11);

                vec_ret c0 = glm::mix(c00, c10, interpos.x);
                vec_ret c1 = glm::mix(c01, c11, interpos.x);
                color = glm::mix(c0, c1, interpos.y);
            }
            else if (dimmensions == 3)
            {
                glm::vec4 pos000 = px_pos;
                glm::vec4 pos100 = px_pos + glm::uvec4(1, 0, 0, 0);
                glm::vec4 pos010 = px_pos + glm::uvec4(0, 1, 0, 0);
                glm::vec4 pos110 = px_pos + glm::uvec4(1, 1, 0, 0);
                glm::vec4 pos001 = px_pos + glm::uvec4(0, 0, 1, 0);
                glm::vec4 pos101 = px_pos + glm::uvec4(1, 0, 1, 0);
                glm::vec4 pos011 = px_pos + glm::uvec4(0, 1, 1, 0);
                glm::vec4 pos111 = px_pos + glm::uvec4(1, 1, 1, 0);

                vec_ret c000, c100, c010, c110, c001, c101, c011, c111;
                _sample_px(pos000, c000);
                _sample_px(pos100, c100);
                _sample_px(pos010, c010);
                _sample_px(pos110, c110);
                _sample_px(pos001, c001);
                _sample_px(pos101, c101);
                _sample_px(pos011, c011);
                _sample_px(pos111, c111);

                vec_ret c00 = glm::mix(c000, c100, interpos.x);
                vec_ret c10 = glm::mix(c010, c110, interpos.x);
                vec_ret c01 = glm::mix(c001, c101, interpos.x);
                vec_ret c11 = glm::mix(c011, c111, interpos.x);

                vec_ret c0 = glm::mix(c00, c10, interpos.y);
                vec_ret c1 = glm::mix(c01, c11, interpos.y);

                color = glm::mix(c0, c1, interpos.z);
            }
        }

        /**
        *   @brief Transformes the uvw-coordinate to pixel-coordinate.
        *   IMPORTANT: EVERY PARAMETER MUST BE 16-BYTE ALIGNED!!!
        *   @param[in] pos: uvw-coordinate
        *   @param[out] _floor: floored pixel-coordinate
        *   @param[out] _fract: Fractional part of the pixel-coordinate.
        */
        inline void combute_image_pos(const glm::vec4* pos, glm::uvec4* _floor, glm::vec4* _fract) const noexcept
        {
            __m128 a = _mm_load_ps((const float*)pos);                         // load image positions
            __m128 b = _mm_cvtepi32_ps(_mm_load_si128((const __m128i*)&this->create_info)); // load image dimmensions and cast to float
            __m128 res = _mm_mul_ps(a, b);                                      // pixel position = position * size (floating point)
            __m128i floor_res = _mm_cvttps_epi32(res);                          // cast with truncation to integer (floor operation)
            __m128 fract_res = _mm_sub_ps(res, _mm_cvtepi32_ps(floor_res));     // fract operation
            _mm_store_si128((__m128i*)_floor, floor_res);
            _mm_store_ps((float*)_fract, a);
        }

        /**
        *   @brief Combutes the address mode for all dimmensions.
        *   @param[in] pos: pixel-coordinate
        *   @param[out] pos2: Pixel-coordinate after address mode computation.
        */
        void combute_address_mode(const glm::uvec4& pos, glm::uvec4& pos2) const noexcept
        {
            pos2.x = combute_address_mode_comp(pos.x, 0);
            pos2.y = combute_address_mode_comp(pos.y, 1);
            pos2.z = combute_address_mode_comp(pos.z, 2);
        }

    public:
        /**
        *   @param[in] filter: filter operation
        *   @param[in] border_color: Color of the border for oversampling.
        */
        explicit Texture(Filter filter = RT_FILTER_NEAREST, const vec_ret& border_color = vec_ret(0.0)) noexcept
        {
            this->filter = filter;
            this->border_color = border_color;
            this->set_address_mode(RT_TEXTURE_ADDRESS_MODE_REPEAT, RT_TEXTURE_ADDRESS_MODE_REPEAT, RT_TEXTURE_ADDRESS_MODE_REPEAT);
        }
        virtual ~Texture(void) {}

        /**
        *   @param[in] u: Address-mode in the u-direction.
        *   @param[in] v: Address-mode in the v-direction.
        *   @param[in] w: Address-mode in the w-direction.
        */
        void set_address_mode(TextureAddressMode u, TextureAddressMode v, TextureAddressMode w) noexcept
        { 
            this->address_mode[0] = u;
            this->address_mode[1] = v;
            this->address_mode[2] = w;
        }

        /** @param[in] filter: filter operation */
        void set_filter(Filter filter) noexcept { this->filter = filter; }

        /** @param[in] border_color: Color of the border for oversampling. */
        void set_border_color(const vec_ret& border_color) noexcept { this->border_color = border_color; }

        /**
        *   @brief Loads the pixels into the texture object.
        *   NOTE: This method does not read the image file!
        *   @param[in] ci: image create info
        *   @param[in] byte_data: pixels
        *   @return image error
        */
        ImageError load(const ImageCreateInfo& ci, const T_src* byte_data) noexcept
        {
            if (byte_data == nullptr) return RT_IMAGE_ERROR_NULL;
            if (ci.width == 0 || ci.height == 0 || ci.depth == 0) return RT_IMAGE_ERROR_ZERO_SIZE;

            this->free();
            this->set_create_info(ci);
            ImageError error = this->create();  // error may return an error code
            if (error != RT_IMAGE_ERROR_NONE) return error;

            T_dst* map = this->map_rdwr();          // get pointer to the image data array
            size_t s = this->count() * ci.channels; // get number of elemnts in the image array

            // iterate through every image array element and convert it from source- to destination-type
            for (size_t i = 0; i < s; i++)
                map[i] = convert_type(byte_data[i]);

            return RT_IMAGE_ERROR_NONE;
        }

        /**
        *   @brief Samples the texture with filter operations applied.
        *   @praram[in] pos: uvw-coordinate
        *   @return Color of the sample.
        *   NOTE: This method can be inheritated and overwritten.
        */
        virtual vec_ret sample(const glm::vec4& pos)
        {
            vec_ret color;
            this->_sample(pos, color);
            return color;
        }
    };

    template<typename T_src, typename T_dst> using Texture1D = Texture<T_src, T_dst, 1>;
    template<typename T_src, typename T_dst> using Texture2D = Texture<T_src, T_dst, 2>;
    template<typename T_src, typename T_dst> using Texture3D = Texture<T_src, T_dst, 3>;

    /**
    *   Load functions for typical image formats:
    *   1) 8-bit (per color channel) image format
    *   2) 16-bit (per color channel) image format
    *   3) floating-point image format
    */
    namespace TextureLoader
    {
        /**
        *   @brief Loads a 8-bit (per color channel) image into a texture object.
        *   @param[out] tex: texture object
        *   @param[in] path: Path to the image.
        *   @param[in] force_channels: Force the loader to load a certain number of color channels (1 - 4).
        *                              Leave it to 0 to not force the loader.
        *   @return image error
        */
        ImageError load(Texture2D<uint8_t, float>& tex, const std::string& path, uint32_t force_channels);

        /**
        *   @brief Loads a 16-bit (per color channel) image into a texture object.
        *   @param[out] tex: texture object
        *   @param[in] path: Path to the image.
        *   @param[in] force_channels: Force the loader to load a certain number of color channels (1 - 4).
        *                              Leave it to 0 to not force the loader.
        *   @return image error
        */
        ImageError load16(Texture2D<uint16_t, float>& tex, const std::string& path, uint32_t force_channels);

        /**
        *   @brief Loads a floating-point image into a texture object.
        *   @param[out] tex: texture object
        *   @param[in] path: Path to the image.
        *   @param[in] force_channels: Force the loader to load a certain number of color channels (1 - 4).
        *                              Leave it to 0 to not force the loader.
        *   @return image error
        */
        ImageError loadf(Texture2D<float, float>& tex, const std::string& path, uint32_t force_channels);
    }
}