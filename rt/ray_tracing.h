/**
* @file     ray_tracing.h
* @brief    Main include-file for the ray-tracing library.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

// include texture
#include "image/texture.h"
#include "image/spherical_map.h"
#include "image/cubemap.h"

// include primitive
#include "primitive/sphere.h"
#include "primitive/distancesphere.h"
#include "primitive/infplane.h"

// include ray tracing
#include "misc/app.h"