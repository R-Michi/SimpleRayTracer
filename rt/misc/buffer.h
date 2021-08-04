/**
* @file     buffer.h
* @brief    Buffer used for the ray-tracing pipeline, that can store primitives.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#pragma once

#include "rt_error.h"
#include "../primitive/primitive.h"
#include <vector>

namespace rt
{
    /**
     *  This class stores primitives and can make the internal primitive buffer aviable by
     *  mapping it (read-only or read-write access).
     *  But keep in mind that the buffer is not thread save. If you decide to stream
     *  data into the buffer by a secondary thread, YOU have to make it multithreading-save!
     */
    class Buffer
    {
    private:
        BufferLayout _layout_info;
        std::vector<Primitive*> _buff;

        // allocates or reallocates buffer memory
        void allocate(void);

    public:
        /**
         *  NOTE: By default the buffer info is set to its default values,
         *  which means the buffer is invalid.
         *  Adding data to the buffer will have no effect, instead a RT_BUFFER_INVALID_INFO will
         *  be returned.
         *  Mapping data will return a 'nullptr'.
         */
        Buffer(void);

        /**
         *  NOTE: Wrong parameters of the BufferLayoutInfo will force
         *  the buffer to be invalid.
         *  @param[in] layout_info: Buffer-layout information.
         */
        explicit Buffer(const BufferLayout& layout_info);

        Buffer(const Buffer& buff);
        Buffer& operator= (const Buffer& buff);
        Buffer(Buffer&& buff);
        Buffer& operator= (Buffer&& buff);

        virtual ~Buffer(void);

        /**
         *  @brief Sets / updates the buffer layout info.
         *  NOTE: This will may cause some changes to the internal
         *  data buffer itself. Be careful if you execute this method,
         *  you have to know what you are doing!
         *  @param[in] layout_info: Buffer-layout information.
         */
        void set_layout(const BufferLayout& layout_info);

        /**
         *  Loads one primitive into the buffer at position @param pos.
         *  @param[in] prim: The primitive to load in.
         *  @param[in] pos: Position to put the primitive in.
         *  @return Error if something went wrong.
         */
        BufferError data(size_t pos, Primitive* prim);

        /**
         *  @brief Loads an array of primitives into the buffer.
         *  Array cannot contain mixed primitives.
         *  @param[in] begin: Offset to the first primitive.
         *  @param[in] count: Number of Primitives to copy into the buffer.
         *  @param[in] prims: The array of primitives.
         *  @return Error if something went wrong.
         */
        BufferError data(size_t begin, size_t count, Primitive* prims);

        /**
         *  @return The the internal array of primitive pointers.
         *              The mapped memory is read write.
         *  NOTE: If the buffer-layout is invalid this function will return 'nullptr'.
         */
        inline Primitive** map_rdwr(void) noexcept
        {return (this->_buff.size() == 0) ? nullptr : this->_buff.data();}

        /**
         *  @return The the internal array of primitive pointers.
         *              The mapped memory is read only.
         *  NOTE: If the buffer-layout is invalid this function will return 'nullptr'.
         */
        inline const Primitive * const * map_rdonly(void) const noexcept
        {return (this->_buff.size() == 0) ? nullptr : this->_buff.data();}

        /** @return Buffer-layout information. */
        inline const BufferLayout& layout(void) const noexcept
        {return this->_layout_info;}

        // Cleares all internal memory.
        void clear(void) noexcept;

        /**
         *  @brief Cleares memory in a given range.
         *  Parameters that would be out of range will be ignored.
         *  @param[in] begin: Begin index to clear.
         *  @param[in] end: End index to clear.
         */
        void clearEXT(size_t begin, size_t end) noexcept;
    };
}