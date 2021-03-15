#ifndef __rt_error_h__
#define __rt_error_h__

#include <cstdint>

namespace rt
{
    enum BufferError : uint16_t
    {
        RT_BUFFER_ERROR_NONE    = 0x0000,
        RT_BUFFER_OVERFLOW      = 0x0001,
        RT_BUFFER_INVALID_INFO  = 0x0002
    };
}

#endif //__rt_error_h__