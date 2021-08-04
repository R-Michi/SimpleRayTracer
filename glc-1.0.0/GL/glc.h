/**
* @file     glc.h
* @brief    Useful functions for OpenGL image dimmension conversions.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef GLC_H_INCLUDED
#define GLC_H_INCLUDED

#define __glc_h__

#include <stdint.h>

namespace gl
{
    namespace convert
    {
        // converts pixel size to OpenGL screen size
        inline float from_pixels_size(int16_t px, uint16_t ref_size)    {return 2.0f * px / ref_size;}
        // converts pixel position x to OpenGL screen position x
        inline float from_pixels_pos_x(int16_t px, uint16_t size_x)     {return 2.0f * px / size_x - 1;}
        // converts pixel position x to OpenGL screen position x
        inline float from_pixels_pos_y(int16_t px, uint16_t size_y)     {return -2.0f * px / size_y + 1;}
        // converts OpenGL screen size to pixel size
        inline int16_t to_pixels_size(float size, uint16_t ref_size)    {return size * ref_size / 2.0f;}
        // converts OpenGL position x to pixel position x
        inline int16_t to_pixels_pos_x(float pos, uint16_t size_x)      {return size_x / 2.0f * (pos + 1);}
        // converts OpenGL position y to pixel position y
        inline int16_t to_pixels_pos_y(float pos, uint16_t size_y)      {return size_y / -2.0f * (pos - 1);}
        // screen pixels to image pixels ratio
        inline float from_img_px(uint16_t screen_px, uint16_t img_px)   {return (float)screen_px / (float)img_px;}
    };
};
#endif // GLC_H_INCLUDED
