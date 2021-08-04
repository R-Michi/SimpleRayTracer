/**
* @file     rt_error.h
* @brief    Error codes for the ray-tracing library.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include <cstdint>

namespace rt
{
    enum BufferError : uint32_t
    {
        RT_BUFFER_ERROR_NONE    = 0,
        RT_BUFFER_ERROR_OVERFLOW      = 1,
    };

    enum ImageError : uint32_t
    {
        RT_IMAGE_ERROR_NONE = 0,
        RT_IMAGE_ERROR_NULL = 1,
        RT_IMAGE_ERROR_OUT_OF_MEMORY = 2,
        RT_IMAGE_ERROR_OUT_OF_RANGE = 3,
        RT_IMAGE_ERROR_ZERO_SIZE = 4
    };
}