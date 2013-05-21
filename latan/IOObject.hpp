#ifndef LATAN_IOOBJECT_HPP_
#define LATAN_IOOBJECT_HPP_

#include <latan/Global.hpp>

LATAN_BEGIN_CPPDECL

namespace IOTypes
{
    typedef enum
    {
        NoType = 0,
        DMat   = 1,
        Sample = 2
    } Type;
}

// Abstract base for IO objects
class IOObject
{
public:
    virtual ~IOObject(void);
    virtual IOTypes::Type IOType(void);
};

LATAN_END_CPPDECL

#endif
