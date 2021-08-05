/**
* @file     framebuffer.h
* @brief    Implementation of a frame-buffer that uses the image class as base.
*           The frame-buffer is used to store the rendered pixel data.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "image.h"

namespace rt
{
    class Framebuffer : public Image2D<uint8_t>
    {
    public:
        Framebuffer(void) : Image2D<uint8_t>() {}
        explicit Framebuffer(const ImageCreateInfo& ci) noexcept : Image2D<uint8_t>(ci) {}
        virtual ~Framebuffer(void) {}
    };
}