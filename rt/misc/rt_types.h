/**
* @file     rt_types.h
* @brief    Type definitions for the ray-tracing library.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include <glm/glm.hpp>

#define RT_INTERSECTION_CONSIDER_INSIDE 0x0001

namespace rt
{
    // ================ CLASSES ================
    class PrimitiveAttribute
    {
    public:
        PrimitiveAttribute(void) {}
        virtual ~PrimitiveAttribute(void) {}
    };

    // ================ STRUCTS ================
    struct ray_t
    {
        glm::vec3 origin;
        glm::vec3 direction;
    };

    struct BufferLayout
    {
        size_t size = 0;    // the number of primitives the buffer can store
        size_t first = 0;   // first primitive that is processed
        size_t last = 0;    // last primitive that is processed
    };

    struct ImageCreateInfo
    {
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t channels;
    };

    struct CubemapCreateInfo
    {
        const char* right;
        const char* left;
        const char* top;
        const char* bottom;
        const char* front;
        const char* back;
    };

    // ================ ENUMS ================
    enum Filter : uint32_t
    {
        RT_FILTER_NEAREST = 0,
        RT_FILTER_LINEAR = 1
    };

    enum TextureAddressMode : uint32_t
    {
        RT_TEXTURE_ADDRESS_MODE_REPEAT = 0,
        RT_TEXTURE_ADDRESS_MODE_MIRRORED_REPEAT = 1,
        RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
        RT_TEXTURE_ADDRESS_MODE_CLAMP_TO_BORDER = 3
    };

    enum CubemapFace : uint32_t
    {
        RT_CUBEMAP_FACE_POSITIVE_X = 0,
        RT_CUBEMAP_FACE_NEGATIVE_X = 1,
        RT_CUBEMAP_FACE_POSITIVE_Y = 2,
        RT_CUBEMAP_FACE_NEGATIVE_Y = 3,
        RT_CUBEMAP_FACE_POSITIVE_Z = 4,
        RT_CUBEMAP_FACE_NEGATIVE_Z = 5,
        RT_CUBEMAP_FACE_RIGHT = RT_CUBEMAP_FACE_POSITIVE_X,
        RT_CUBEMAP_FACE_LEFT = RT_CUBEMAP_FACE_NEGATIVE_X,
        RT_CUBEMAP_FACE_TOP = RT_CUBEMAP_FACE_POSITIVE_Y,
        RT_CUBEMAP_FACE_BOTTOM = RT_CUBEMAP_FACE_NEGATIVE_Y,
        RT_CUBEMAP_FACE_FRONT = RT_CUBEMAP_FACE_POSITIVE_Z,
        RT_CUBEMAP_FACE_BACK = RT_CUBEMAP_FACE_NEGATIVE_Z
    };
}