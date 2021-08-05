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
#include <iostream>

using namespace rt;

Primitive::Primitive(void) noexcept
{
    this->attrib = nullptr;
}

Primitive::~Primitive(void)
{
    delete this->attrib;
}

Primitive::Primitive(const PrimitiveAttribute& attrib) noexcept : Primitive()
{
    this->set_attribute(attrib);
}

void Primitive::set_attribute(const PrimitiveAttribute* attrib) noexcept
{
    if (attrib == nullptr) return;
    if(this->attrib != nullptr) delete this->attrib;    // delete old attribute
    this->attrib = attrib->clone_dynamic();             // allocate new one
}

void Primitive::set_attribute(const PrimitiveAttribute& attrib) noexcept
{
    this->set_attribute(&attrib);
}
