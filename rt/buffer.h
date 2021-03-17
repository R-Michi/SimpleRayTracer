#ifndef __rt_buffer_h__
#define __rt_buffer_h__

#include "rt_types.h"
#include "rt_error.h"
#include "primitive.h"

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
        BufferLayoutInfo _layout_info;
        std::vector<Primitive*> _buff;

        // allocates or reallocates buffer memory
        void allocate(void);

    public:
        /**
         *  Default constructor of the buffer.
         *  NOTE: By default the buffer info is set to its default values,
         *  which means the buffer is invalid.
         *  Adding data to the buffer will have no effect, instead a RT_BUFFER_INVALID_INFO will
         *  be returned.
         *  Mapping data will return a 'nullptr'.
         */
        Buffer(void);

        /**
         *  Constructor which specifies the buffer layout.
         *  NOTE: Wrong parameters of the BufferLayoutInfo will force
         *  the buffer to be invalid.
         *  @param layout_info -> Buffer-layout information.
         */
        explicit Buffer(const BufferLayoutInfo& layout_info);

        /**
         *  Copies an object into the own instance.
         *  @param buff -> Buffer to copy.
         */
        Buffer(const Buffer& buff);
        Buffer& operator= (const Buffer& buff);

        /**
         *  Moves an object into the own instance.
         *  @param buff -> Buffer to move.
         */
        Buffer(Buffer&& buff);
        Buffer& operator= (Buffer&& buff);

        /**
         *  The destructor will automatically clear dynamic memory within the buffer.
         *  This also includes mapped buffers.
         */
        virtual ~Buffer(void);

        /**
         *  Sets / updates the buffer layout info.
         *  NOTE: This will may cause some changes to the internal
         *  data buffer itself. Be careful if you execute this method,
         *  you have to know what you are doing!
         *  @param layout_info -> Buffer-layout information.
         */
        void set_layout_info(const BufferLayoutInfo& layout_info);

        /**
         *  Loads one primitive into the buffer at position @param pos.
         *  @param prim -> The primitive to load in.
         *  @param pos -> Position to put the primitive in.
         *  @return -> Error if something went wrong.
         */
        BufferError data(size_t pos, Primitive* prim);

        /**
         *  Loads an array of primitives into the buffer.
         *  Array cannot contain mixed primitives.
         *  @param begin -> Offset to the first primitive.
         *  @param count -> Number of Primitives to copy into the buffer.
         *  @param prims -> The array of primitives.
         *  @return -> Error if something went wrong.
         */
        BufferError data(size_t begin, size_t count, Primitive* prims);

        /**
         *  @return ->  The the internal array of primitive pointers.
         *              The mapped memory is read write.
         *  NOTE: If the buffer-layout is invalid this function will return 'nullptr'.
         */
        inline Primitive** map_rdwr(void) noexcept
        {return (this->_buff.size() == 0) ? nullptr : this->_buff.data();}

        /**
         *  @return ->  The the internal array of primitive pointers.
         *              The mapped memory is read only.
         *  NOTE: If the buffer-layout is invalid this function will return 'nullptr'.
         */
        inline const Primitive * const * map_rdonly(void) const noexcept
        {return (this->_buff.size() == 0) ? nullptr : this->_buff.data();}

        /// @return -> Buffer-layout information.
        inline const BufferLayoutInfo& info(void) const noexcept
        {return this->_layout_info;}

        // Cleares all internal memory.
        void clear(void) noexcept;

        /**
         *  Cleares memory in a given range.
         *  Parameters that would be out of range will be ignored.
         *  @param begin -> Begin index to clear.
         *  @param end -> End index to clear.
         */
        void clearEXT(size_t begin, size_t end) noexcept;
    };
}

#endif //__rt_buffer_h__