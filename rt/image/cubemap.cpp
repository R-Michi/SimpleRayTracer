/**
* @file     cubemap.cpp
* @brief    Contains useful functions to quickly load cubemaps with common image formats.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "cubemap.h"

using namespace rt;

ImageError TextureLoader::load_cube(Cubemap<uint8_t, float>& cubemap, const CubemapCreateInfo& cci, uint32_t force_channels)
{
    const char* const* const paths = (const char* const*)&cci;

    uint8_t* data;
    int w, h, c;
    ImageCreateInfo image_ci = {};
    ImageError error;

    for (uint32_t i = 0; i < 6; i++)
    {
        data = stbi_load(paths[i], &w, &h, &c, force_channels);

        image_ci.width = (uint32_t)w;
        image_ci.height = (uint32_t)h;
        image_ci.depth = 1;
        image_ci.channels = (force_channels == 0) ? (uint32_t)c : force_channels;

        error = cubemap.load(image_ci, static_cast<CubemapFace>(i), data);
        if (error != RT_IMAGE_ERROR_NONE) return error;
    }
    return RT_IMAGE_ERROR_NONE;
}

ImageError TextureLoader::load_cube16(Cubemap<uint16_t, float>& cubemap, const CubemapCreateInfo& cci, uint32_t force_channels)
{
    const char* const* const paths = (const char* const*)&cci;

    uint16_t* data;
    int w, h, c;
    ImageCreateInfo image_ci = {};
    ImageError error;

    for (uint32_t i = 0; i < 6; i++)
    {
        data = stbi_load_16(paths[i], &w, &h, &c, force_channels);

        image_ci.width = (uint32_t)w;
        image_ci.height = (uint32_t)h;
        image_ci.depth = 1;
        image_ci.channels = (force_channels == 0) ? (uint32_t)c : force_channels;

        error = cubemap.load(image_ci, static_cast<CubemapFace>(i), data);
        if (error != RT_IMAGE_ERROR_NONE) return error;
    }
    return RT_IMAGE_ERROR_NONE;
}

ImageError TextureLoader::load_cubef(Cubemap<float, float>& cubemap, const CubemapCreateInfo& cci, uint32_t force_channels)
{
    const char* const* const paths = (const char* const*)&cci;

    float* data;
    int w, h, c;
    ImageCreateInfo image_ci = {};
    ImageError error;

    for (uint32_t i = 0; i < 6; i++)
    {
        data = stbi_loadf(paths[i], &w, &h, &c, force_channels);

        image_ci.width = (uint32_t)w;
        image_ci.height = (uint32_t)h;
        image_ci.depth = 1;
        image_ci.channels = (force_channels == 0) ? (uint32_t)c : force_channels;

        error = cubemap.load(image_ci, static_cast<CubemapFace>(i), data);
        if (error != RT_IMAGE_ERROR_NONE) return error;
    }
    return RT_IMAGE_ERROR_NONE;
}