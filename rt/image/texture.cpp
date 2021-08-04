/**
* @file     texture.cpp
* @brief    Contains useful functions to quickly load textures with common image formats.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "texture.h"

using namespace rt;

ImageError TextureLoader::load(Texture2D<uint8_t, float>& tex, const std::string& path, uint32_t force_channels)
{
    int w, h, c;
    uint8_t* data = stbi_load(path.c_str(), &w, &h, &c, force_channels);

    ImageCreateInfo image_ci = {};
    image_ci.width = (uint32_t)w;
    image_ci.height = (uint32_t)h;
    image_ci.depth = 1;
    image_ci.channels = (force_channels == 0) ? (uint32_t)c : force_channels;

    ImageError error = tex.load(image_ci, data);
    if (error != RT_IMAGE_ERROR_NONE) return error;

    stbi_image_free(data);
    return RT_IMAGE_ERROR_NONE;
}

ImageError TextureLoader::load16(Texture2D<uint16_t, float>& tex, const std::string& path, uint32_t force_channels)
{
    int w, h, c;
    uint16_t* data = stbi_load_16(path.c_str(), &w, &h, &c, force_channels);

    ImageCreateInfo image_ci = {};
    image_ci.width = (uint32_t)w;
    image_ci.height = (uint32_t)h;
    image_ci.depth = 1;
    image_ci.channels = (force_channels == 0) ? (uint32_t)c : force_channels;

    ImageError error = tex.load(image_ci, data);
    if (error != RT_IMAGE_ERROR_NONE) return error;

    stbi_image_free(data);
    return RT_IMAGE_ERROR_NONE;
}

ImageError TextureLoader::loadf(Texture2D<float, float>& tex, const std::string& path, uint32_t force_channels)
{
    int w, h, c;
    float* data = stbi_loadf(path.c_str(), &w, &h, &c, force_channels);

    ImageCreateInfo image_ci = {};
    image_ci.width = (uint32_t)w;
    image_ci.height = (uint32_t)h;
    image_ci.depth = 1;
    image_ci.channels = (force_channels == 0) ? (uint32_t)c : force_channels;

    ImageError error = tex.load(image_ci, data);
    if (error != RT_IMAGE_ERROR_NONE) return error;

    stbi_image_free(data);
    return RT_IMAGE_ERROR_NONE;
}