/**
* @file     primitive.cpp
* @brief    Basic methods for the primitive.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "primitive.h"

using namespace rt;

Primitive::Primitive(void) noexcept
{
    this->attrib = nullptr;
}

Primitive::Primitive(const PrimitiveAttribute* attrib) noexcept
{
    this->attrib = attrib;
}

void Primitive::set_attribute(const PrimitiveAttribute* attrib) noexcept
{
    this->attrib = attrib;
}