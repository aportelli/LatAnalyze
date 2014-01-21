#ifndef LATAN_IOOBJECT_HPP_
#define LATAN_IOOBJECT_HPP_

#include <latan/Global.hpp>

LATAN_BEGIN_CPPDECL

// Abstract base for IO objects
class IOObject
{
public:
    enum IOType
    {
        noType = 0,
        dMat   = 1,
        sample = 2
    };
public:
    virtual ~IOObject(void)      = 0;
    virtual IOType getType(void) = 0;
};

LATAN_END_CPPDECL

#endif
