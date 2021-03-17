#include "../buffer.h"

#include <malloc.h>

using namespace rt;

Buffer::Buffer(void)
{
    this->_layout_info  = BufferLayoutInfo();
}

Buffer::Buffer(const BufferLayoutInfo& layout_info) : Buffer()
{
    this->set_layout_info(layout_info);
}

Buffer::Buffer(const Buffer& buff)
{
    *this = buff;
}

Buffer& Buffer::operator= (const Buffer& buff)
{
    this->set_layout_info(buff._layout_info);           // copy layout information
    this->clear();                                      // clear own buffer memory
    for(size_t i = 0; i < buff._layout_info.size; i++)  // copy other instance's memory into own memory    
    { 
        if(buff._buff[i] != nullptr)
            this->_buff[i] = buff._buff[i]->clone_dynamic();
    }
    return *this;
}

Buffer::Buffer(Buffer&& buff)
{
    *this = std::move(buff);
}

Buffer& Buffer::operator= (Buffer&& buff)
{
    this->set_layout_info(buff._layout_info);           // copy layout information
    this->clear();                                      // clear own memory
    for(size_t i = 0; i < buff._layout_info.size; i++)  // copy other instance's memory into own memory   
        this->_buff[i] = buff._buff[i]->clone_dynamic();
    buff.set_layout_info(BufferLayoutInfo());           // set other instance's layout information to default values, other instance's memory gets cleared automatically
    return *this;
}

Buffer::~Buffer(void)
{
    this->clear();  // clear all stored primitives
}

void Buffer::set_layout_info(const BufferLayoutInfo& layout_info)
{
    this->_layout_info = layout_info;
    this->allocate();   // reallocate memory
}

BufferError Buffer::data(size_t pos, Primitive* prim)
{
    if(pos >= this->_buff.size())
        return BufferError::RT_BUFFER_OVERFLOW;

    if(this->_buff[pos] != nullptr)
    {
        delete(this->_buff[pos]);                    // delete actual primitive that is at this position
        this->_buff[pos] = nullptr;
    }
    if(prim != nullptr)
        this->_buff[pos] = prim->clone_dynamic();    // copy primitive to this position.
    return BufferError::RT_BUFFER_ERROR_NONE;
}

BufferError Buffer::data(size_t begin, size_t count, Primitive* prims)
{
    const size_t end = begin + count;

    int8_t* _prims = (int8_t*)prims;  // cast to byte pointer
    for(size_t i = begin; i < end; i++)
    {
        BufferError err = this->data(i, (Primitive*)_prims);
        if(err != BufferError::RT_BUFFER_ERROR_NONE)
            return err;

        _prims += ((Primitive*)_prims)->get_sizeof();
    }
    return BufferError::RT_BUFFER_ERROR_NONE;
}

void Buffer::allocate(void)
{
    const size_t old_size = this->_buff.size();                 // save old size of the buffer
    this->clearEXT(this->_layout_info.size, old_size);          // clear memory that would be out of bounds
    this->_buff.resize(this->_layout_info.size);                // resize buffer

    for(size_t i = old_size; i < this->_buff.size(); i++)       // initialize every new element with nullptr
        this->_buff[i] = nullptr;
}

void Buffer::clear(void) noexcept
{
    this->clearEXT(0, this->_buff.size());
}

void Buffer::clearEXT(size_t begin, size_t end) noexcept
{
    for(size_t i = begin; i < end && i < this->_buff.size(); i++)
    {
        if(this->_buff[i] != nullptr)
        {
            delete(this->_buff[i]);
            this->_buff[i] = nullptr;
        }
    }
}