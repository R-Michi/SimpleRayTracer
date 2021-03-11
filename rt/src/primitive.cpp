#include "../primitive.h"

using namespace rt;

Primitive::Primitive(void) noexcept
{
    this->_mtl = {{0.0f, 0.0f, 0.0f}, 1.0f, 0.0f, 1.0f};
}

Primitive::Primitive(const Material& mtl) noexcept
{
    this->_mtl = mtl;
}

void Primitive::set_material(const Material& mtl) noexcept
{
    this->_mtl = mtl;
}